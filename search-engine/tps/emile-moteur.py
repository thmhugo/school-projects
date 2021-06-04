#!/usr/bin/env python

# from sklearn.metrics.pairwise import euclidean_distances
from sklearn.feature_extraction.text import CountVectorizer
from stop_words import get_stop_words
from nltk.tokenize import sent_tokenize

import time
import distance
import sys
import glob
import re
import numpy as np

stop_words = set(get_stop_words('french'))

listdocs = []
listnames = []

t1 = time.process_time()
for docname in glob.glob('Documents/*.txt'):
    doctxt = ''
    for ligne in open(docname):
        doctxt += ligne
    listdocs.append(doctxt)
    listnames.append(docname[10:-4])

vectorizer = CountVectorizer()
matrixdocs = vectorizer.fit_transform(listdocs)
features_names = vectorizer.get_feature_names()
matrixdocs_array = matrixdocs.toarray()
total_freqs = [np.sum(np.array(a)) for a in matrixdocs_array]
elapsed_time = abs(time.process_time() - t1)
print("[info]: corpus analysed in " + str(elapsed_time))


def get_index(word):
    try:
        return features_names.index(word)
    except:
        best = 0
        mindiff = -1
        minlen = -1
        for i in range(len(features_names)):
            dist = distance.levenshtein(word, features_names[i])
            if mindiff == -1 or dist <= mindiff:
                if minlen == -1 or dist < mindiff or (minlen >= len(
                        features_names[i])):
                    mindiff = dist
                    best = i
                    minlen = len(features_names[i])
        return best


def get_best_match(index):
    best = 0
    maxfreq = -1
    for i in range(len(listnames)):
        freq = matrixdocs_array[i][index]
        if freq >= maxfreq:
            maxfreq = freq
            best = i
    return best, maxfreq


def get_best_match2(keywords):
    best = 0
    maxfreq = -1
    feat_words = []
    kword_indexes = []
    freqs = [0 for i in range(len(listnames))]

    i = 1
    for w in keywords:
        kword_index = get_index(w.lower())
        word = features_names[kword_index]
        if word not in stop_words:
            feat_words.append(word)
            kword_indexes.append((i, kword_index))
            i += 1

    for i in range(len(listnames)):
        for (g, j) in kword_indexes:
            freqs[i] += matrixdocs_array[i][j] / (total_freqs[i])  # * g
        if freqs[i] >= maxfreq:
            maxfreq = freqs[i]
            best = i

    return best, freqs, feat_words


def print_res(best, freqs, feat_words, elapsed_time):
    time = "Elapsed time: " + str(truncate(elapsed_time, 4))
    used_words = "   Used word: " + str(feat_words)
    matched_file = "Matched file: " + listnames[best]
    max_len = (len(used_words) if len(used_words) > len(matched_file) else
               len(matched_file)) + 4
    print("+" + "-" * (max_len - 2) + "+")
    print("| " + used_words.ljust(max_len - 4) + " |")
    print("| " + matched_file.ljust(max_len - 4) + " |")
    print("| " + time.ljust(max_len - 4) + " |")
    print("+" + "-" * (max_len - 2) + "+")
    hm_freqs = {k: freqs[k] for k in range(len(freqs))}
    hm_freqs = {
        k: v
        for k, v in sorted(hm_freqs.items(), key=lambda x: x[1], reverse=True)
    }
    if len(sys.argv) >= 2 and sys.argv[1].strip() == "--verbose":
        for k, v in hm_freqs.items():
            if v != 0:
                print("| " + str(truncate(v, 4)).ljust(7) + "| " +
                      listnames[k][:max_len - 13].ljust(max_len - 13) + " |")
        print("+" + "-" * (max_len - 2) + "+")


def truncate(num, n):
    integer = int(num * (10**n)) / (10**n)
    return float(integer)


def sent_contain_word(sent, words):
    sent = sent.lower()
    for w in words:
        if w in sent:
            return True
    return False


def word_contain_featword(word, words):
    word = word.lower()
    for w in words:
        if w in word:
            return True
    return False


def print_quotes(best, feat_words):
    print("\nQuotes:")
    with open("Documents/" + listnames[best] + ".txt") as file:
        for s in sent_tokenize(file.read().replace("\n", "")):
            if sent_contain_word(s, feat_words):
                print("<< ", end="")
                for w in s.split(" "):
                    if word_contain_featword(w, feat_words):
                        print("\033[0;35m" + w + "\033[0m", end=" ")
                    else:
                        print(w, end=" ")
                print(" >>\n")


while True:
    keywords = input("search> ").split(" ")
    if len(keywords) > 0:
        if "exit" in keywords[0]:
            break
        t1 = time.process_time()
        best, freqs, feat_words = get_best_match2(keywords)
        elapsed_time = abs(time.process_time() - t1)
        print_res(best, freqs, feat_words, elapsed_time)
        print_quotes(best, feat_words)
