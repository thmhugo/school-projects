from prompt_toolkit import print_formatted_text as print
from prompt_toolkit.formatted_text import HTML
from prompt_toolkit.auto_suggest import AutoSuggestFromHistory
from prompt_toolkit.styles import Style
from prettytable import PrettyTable
from nltk.tokenize import sent_tokenize
import embedder

styles = Style.from_dict({
    'cmd': 'ansicyan bold',
    'args': 'ansiyellow',
    'text': 'italic',
    'flags': 'ansiyellow italic',
    'quote': 'ansipurple italic',
    'err': 'ansired',
    'warn': '#ffb100',
})


def bottom_toolbar():
    """Create a the bottom toolbar"""
    return HTML('🐍 <i>Snake_search</i> <aa bg="ansigreen"><b>v2.0</b></aa> '
                '| Available commands:'
                ' <aa bg="ansicyan"><b>help</b></aa>,'
                ' <aa bg="ansicyan"><b>benchmark</b></aa>,'
                ' <aa bg="ansicyan"><b>download</b></aa>,'
                ' <aa bg="ansicyan"><b>search</b></aa>,'
                ' <aa bg="ansicyan"><b>quote</b></aa>')


def prompt(session):
    """Print the custom prompt"""
    return session.prompt(HTML("<b><ansigreen>></ansigreen></b> "),
                          bottom_toolbar=bottom_toolbar,
                          auto_suggest=AutoSuggestFromHistory(),
                          style=styles)


def print_help():
    """Print the help message"""
    print(HTML(
        "Available commands:\n\n"
        "  <cmd>search</cmd> <args>string</args> <flags>FLAGS</flags>   "
        "\tsearch with the given query\n"
        "  <cmd>quote</cmd> <args>string</args>              "
        "\tprint quotes of the last search in the given document, or if omitted the last best match\n"
        "  <cmd>benchmark</cmd> <args>string</args> <flags>FLAGS</flags>\tbenchmark a specified strategy, by default the <i>./benchmark/queries.yaml</i> file is used.\n"
        "  <cmd>download</cmd>               \tdownload <i>punkt</i> and <i>stopwords</i> nltk models\n"
        "  <cmd>help</cmd>                   \tprint the help message\n"
        "  <cmd>exit</cmd>                   \tstop the session\n\n"
        "Available flags:\n\n"
        "  <cmd>--verbose</cmd>        \tenable the verbose mode\n"
        "  <cmd>--no-sim</cmd>        \tdisable query enrichment\n"
        "  <cmd>--threshold</cmd>=<args>int</args>  \tspecified a minimum treshold (in percent) for keeping a query word\n"
        "  <cmd>--strategy</cmd>=<args>string</args>\tspecified the preprocessing strategy to use\n\n"
        "Available strategies:\n\n"
        "  <cmd>embeddings</cmd> \tuse an embeddings model for query words correction (default strategy)\n"
        "  <cmd>spellcheck</cmd> \tuse the <i>spellchecker</i> librairie\n"
        "  <cmd>levenshtein</cmd>\tuse the nearest corpus's features according the levenshtein distance\n"
    ),
          style=styles)


def print_init_infos(elapsed_times, corpus, is_loading_emb_model):
    print(HTML("                  <text>indexing:</text> " +
               str(len(corpus.doc_names)) + " <text>documents in </text>" +
               str(elapsed_times[0]) + " <text>s</text>"),
          style=styles)
    if is_loading_emb_model:
        print(HTML(
            "                   <text>loading:</text> 1 <text>model (embeddings) in </text>"
            + str(elapsed_times[1]) + " <text>s</text>"),
              style=styles)
    print(HTML("<text>TF-IDF matrix dimensions:</text> " +
               str(len(corpus.tfidf)) + "<text>x</text>" +
               str(len(corpus.tfidf[0]))),
          style=styles)

    if not is_loading_emb_model:
        print(HTML(
            "<warn>No model found at <i>'" + embedder.MODEL_PATH +
            "'</i>, the <b>embeddings</b> strategy could not be used.</warn>"),
              style=styles)


def print_error(msg):
    """Prints a msg with the error style"""
    print(HTML("<err>" + msg + "</err>"), style=styles)


def print_query_res(best_matches,
                    query,
                    elapsed_time,
                    threshold,
                    strategy,
                    verbose=False):
    """Prints the response of `get_best_matches(query)`"""
    if 0 == len(best_matches):
        print("No documents found..")
    else:
        if verbose:
            print(HTML("\n      <text>Used query</text>: " + str(query)),
                  style=styles)
            print(HTML("   <text>Used strategy</text>: " + str(strategy)),
                  style=styles)
            print(HTML("   <text>Threshold (%)</text>: " + str(threshold)),
                  style=styles)
            print(HTML("<text>Process time (s)</text>: " + str(elapsed_time) +
                       "\n"),
                  style=styles)

        ptable = PrettyTable()
        ptable.field_names = ["Document", "Sum of TF-IDF scores"]
        ptable.align["Document"] = "l"
        for name, tfidf_sum in best_matches.items():
            if tfidf_sum >= 0.01:
                ptable.add_row([
                    name,
                    round(tfidf_sum, 4),
                ])

        ptable.reversesort = True
        ptable.sortby = "Sum of TF-IDF scores"
        if verbose:
            ptable.align["Sum of TF-IDF scores"] = "c"
            print(
                ptable.get_string(fields=["Document", "Sum of TF-IDF scores"]))
        else:
            print(ptable.get_string(fields=["Document"], header=False))


def print_quotes(doc, feat_words):
    """Prints quotes of features in a given document"""
    with open(doc) as file:
        print(HTML("<i>" + doc + "</i>:\n"))
        for s in sent_tokenize(file.read().replace("\n", "")):
            if sent_contain_word(s, feat_words):
                print("<< ", end="")
                for w in s.split(" "):
                    if word_contain_featword(w, feat_words):
                        print(HTML("<quote>" + w + "</quote>"),
                              end=" ",
                              style=styles)

                    else:
                        print(w, end=" ")
                print(" >>\n")


def sent_contain_word(sent, words):
    """Verifies if a given sentance contains at least one of the given words."""
    sent = sent.lower()
    for w in words:
        if w in sent:
            return True
    return False


def word_contain_featword(word, words):
    """Verifies if a given word contains as subword a at least one of the given words."""
    word = word.lower()
    for w in words:
        if w in word:
            return True
    return False
