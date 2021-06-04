import re
import glob
from sklearn.feature_extraction.text import TfidfVectorizer
from nltk import word_tokenize
from nltk.stem.snowball import FrenchStemmer
from nltk.corpus import stopwords


class LemmaTokenizer:
    """Tokenizer used with the `TfidfVectorizer`."""
    punctuation = [",", ".", ";", ":", '"', "``", "''", "`"]

    def __init__(self):
        self.wnl = FrenchStemmer()

    def __call__(self, doc):
        return [
            self.wnl.stem(t) for t in word_tokenize(doc)
            if t not in self.punctuation
        ]


class Indexer:
    """Indexes a list of .txt files"""
    doc_content = []
    matrix = []
    doc_names = []

    def index(self, path):
        """
        Creates a corpus according a given path.

        Arguments
        ---------
        path: str
            Path to the directory that contains text files to build the corpus from.

        Returns
        -------
        Corpus:
            An instantiated corpus.
        """
        self.__read_corpus(path)
        vectorizer = TfidfVectorizer()
        self.matrix = vectorizer.fit_transform(self.doc_content)
        self.features = vectorizer.get_feature_names()
        return Corpus(self.features, self.matrix.toarray(), self.doc_names,
                      self.doc_content)

    def __read_corpus(self, path):
        """
        Fills self.doc_content and self.doc_names from the given directory path.

        Parameters
        ----------
        path: str
            Path to the directory that contains text files to build the corpus from.
        """
        for docname in glob.glob(path + "*.txt"):
            doctxt = ""
            for ligne in open(docname):
                doctxt += ligne
            self.doc_content.append(doctxt)
            # Add the file name according to the file path.
            self.doc_names.append(docname.split("/")[-1].split(".")[0])


class Corpus:
    """
    Stores informations about a text corpus needed for query preocessing.

    features: list(str)
        List of corpus's features doc_names.
    tfidf: sparse matrix of (n_samples, n_features)
        Tf-idf-weighted document-term matrix.
    doc_names: list(str)
        List of document's names.
    doc_content:
        List of document's contents.
    """
    def __init__(self, features, tfidf, doc_names, doc_content):
        self.features = features
        self.tfidf = tfidf
        self.doc_names = doc_names
        self.doc_content = doc_content

        frenchStopwords = set(stopwords.words("french"))
        tokenizer = LemmaTokenizer()
        tokenStop = tokenizer(" ".join(frenchStopwords))
        self.vectorizer = TfidfVectorizer(stop_words=tokenStop,
                                          tokenizer=tokenizer)
