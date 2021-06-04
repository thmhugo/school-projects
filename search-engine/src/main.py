#!/usr/bin/env python3
from threading import Thread
from indexer import Indexer
from request_manager import RequestManager, default_threshold
from prompt_toolkit import PromptSession
from prompt_toolkit.shortcuts import ProgressBar
from prompt_toolkit.formatted_text import HTML
from prompt_toolkit.patch_stdout import patch_stdout
import cli
import time
import nltk
import re
import yaml

# Path the corpus directory containing all text files.
corpus_path = "./corpus/"
# Path the YAML test file containing expected output for a given query.
query_benchmark_file = "./benchmark/queries.yaml"


def safe_main():
    """Program entry point"""
    try:
        main()
    except KeyboardInterrupt:
        ()
    except EOFError:
        ()
    except LookupError:
        cmd_download()
        print("`nltk` models downloaded. Restarting.")
        safe_main()


def main():
    """Program entry point, can raise exceptions"""
    session = PromptSession()

    indexing_time, corpus = get_process_time(
        lambda: Indexer().index(corpus_path))
    loading_time, rq_manager = get_process_time(lambda: RequestManager(corpus))
    cli.print_init_infos([indexing_time, loading_time], corpus,
                         rq_manager.model_is_loaded)

    best_matches = dict()
    query = []

    text = cli.prompt(session)
    while text.lower() != "exit":
        i = text.find(' ')
        cmd = text[:i] if i != -1 else text
        msg = text[i + 1:] if i != -1 else ""

        if "help" == cmd:
            cli.print_help()
        elif "download" == cmd:
            cmd_download()
        elif "quote" == cmd:
            cmd_quote(query, best_matches, msg, corpus)
        elif "search" == cmd:
            query, best_matches = cmd_search(rq_manager, msg)
        elif "benchmark" == cmd:
            cmd_safe_benchmark(rq_manager, msg)
        else:
            cli.print_error("Unkown command")
        text = cli.prompt(session)


def cmd_download():
    """Download `nltk` models."""
    nltk.download("punkt")
    nltk.download("stopwords")


def cmd_quote(query, best_matches, document, corpus):
    """Prints quotes of the last search in the given `document`."""
    if 0 == len(document):
        if 0 == len(best_matches):
            cli.print_error("No search performed yet")
        else:
            doc_name = sorted(best_matches, key=best_matches.get)[-1]
            cli.print_quotes(corpus_path + doc_name + ".txt", query)
    elif document not in corpus.doc_names:
        cli.print_error("'" + document +
                        "' doesn't correspond to any document")
    else:
        cli.print_quotes(corpus_path + document + ".txt", query)


# Available strategies.
strategies = ["spellcheck", "embeddings", "levenshtein"]
# Compiled regex to match the specified strategy.
re_strategy_flag = re.compile(".*--strategy=(" + "|".join(strategies) + ").*")
# Compiled regex to match the specified threshold value.
re_threshold_flag = re.compile(".*--threshold=(\\d+).*")


def get_strategy(str):
    """Returns the specified strategy if it exists, otherwise 'default'."""
    strategy = "embeddings"
    if "--strategy=" in str:
        matched = re_strategy_flag.match(str)
        if matched is not None:
            strategy = matched.group(1)
        else:
            cli.print_error(
                'Invalid strategy, the default one will be used ("embeddings").'
            )
    return strategy


def get_threshold(str):
    """Returns the specified threshold if it exists, otherwise 0."""
    threshold = default_threshold
    if "--threshold=" in str:
        matched = re_threshold_flag.match(str)
        if matched is not None:
            threshold = int(matched.group(1))
            if 0 > threshold or 100 < threshold:
                cli.print_error(
                    f"Invalid threshold: {threshold}, {default_threshold} will be used."
                )
                threshold = default_threshold
    return threshold


def cmd_search(rq_manager, input_query):
    """Performs a search according the given `query`."""
    strategy = get_strategy(input_query)
    threshold = get_threshold(input_query)
    search_similar = "--no-sim" not in input_query
    elapsed_time, (
        best_matches,
        query) = get_process_time(lambda: rq_manager.get_best_matches(
            input_query, threshold, search_similar, strategy))
    cli.print_query_res(best_matches, query, elapsed_time, threshold, strategy,
                        ("--verbose" in input_query))

    return query, best_matches


def cmd_safe_benchmark(rq_manager, args):
    """Manages [args] before benchmarking the provided strategy (if not, 'default' is used)."""
    strategy = get_strategy(args)
    threshold = get_threshold(args)

    # Gets the test file
    test_file = query_benchmark_file
    args = args.split()
    for arg in args:
        if "--" not in arg:
            test_file = arg
            break

    # Benchmarks
    try:
        test_file = open(test_file, "r")
        cmd_benchmark(rq_manager, test_file, threshold, strategy)
    except FileNotFoundError:
        cli.print_error("File not found: " + test_file)


def cmd_benchmark(rq_manager, test_file, threshold, strategy):
    """Runs parallel benchmarking."""
    with ProgressBar(title=HTML(
            'Testing the <ansiyellow>' + strategy +
            '</ansiyellow> strategy with a threshold of <ansiyellow>' +
            str(threshold) + '%</ansiyellow>')) as pb:
        threads = list()
        return_values = list()
        for case in yaml.safe_load(test_file):

            def test_subroutine(return_values):
                """Runs in a thread in order to benchmark one case."""
                err = dict()
                case_total_tests = 0
                case_total_success = 0
                for case_name, case_tests in case.items():
                    for i in pb(range(len(case_tests)),
                                label=HTML('case <i>' + case_name + '</i>: ')):
                        test = case_tests[i]
                        case_total_tests += 1
                        query = test['input']
                        actual = rq_manager.get_processed_query(
                            test['input'].lower().split(), threshold, strategy)
                        expected = test['output']

                        if actual != expected:
                            if case_name not in err.keys():
                                err[case_name] = [(query, expected, actual)]
                            else:
                                err[case_name].append(
                                    (query, expected, actual))
                        else:
                            case_total_success += 1
                    return_values.append(
                        (case_name, case_total_success, case_total_tests, err))

            t = Thread(target=test_subroutine,
                       kwargs={"return_values": return_values},
                       daemon=True)
            t.start()
            threads.append(t)

        for t in threads:
            while t.is_alive():
                t.join()

        print_benchmark_res(return_values)


def print_benchmark_res(return_values):
    """Prints formatted benchmark results."""
    total_tests = 0
    total_success = 0
    with patch_stdout():
        for name, success, total, err in return_values:
            print("\nResult [" + name + "]: " + str(success) + "/" +
                  str(total) + "\n")
            if name in err.keys():
                for query, expected, actual in err[name]:
                    print('  \033[31m FAIL: "' + query + '": expected (' +
                          str(expected) + ') got (' + str(actual) + ')\033[0m')
            total_tests += total
            total_success += success
        print("\n\033[0;1mSCORE [" +
              str(round(total_success / total_tests, 2)) + "]: " +
              str(total_success) + "/" + str(total_tests) + "\033[0m")


def get_process_time(f):
    """"Calculates the process time of `f` call"""
    t1 = time.process_time()
    res = f()
    elapsed_time = abs(time.process_time() - t1)
    return round(elapsed_time, 4), res


if __name__ == "__main__":
    safe_main()
