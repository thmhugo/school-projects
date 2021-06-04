import distance
from spellchecker import SpellChecker
from nltk.corpus import stopwords
from embedder import Embedder

# Default threshold value
default_threshold = 50


class RequestManager:
    """Manages queries on a given `Corpus`"""
    def __init__(self, corpus):
        self.corpus = corpus
        self.spellchecker = SpellChecker(language='fr', distance=5)
        try:
            self.embedder = Embedder()
            self.model_is_loaded = True
        except FileNotFoundError:
            self.model_is_loaded = False

    def get_best_matches(self, query, threshold, add_similar, strategy):
        """Processes a query in order to find best matches in the `self.corpus`.

        Parameters
        ----------
        query: str
            The query to process.
        threshold: int
            The minimum similarity threshold.
        add_similar: bool
            If sets to true, adds similar word to the query.
        strategy: str
            The preprocessing strategy to use.

        Returns
        -------
        dict(str: float), array(str)
            The dictionary stores in keys document names and in values tuples
            the sum of TF-IDF scores.
        """
        best_matches = dict()
        query = self.get_processed_query(query.lower().split(), threshold,
                                         strategy)
        tfidf = self.corpus.tfidf

        similar = []

        def search(index, add_similar=False):
            weigth = 1
            for i in range(len(tfidf)):
                if tfidf[i][index] > 0:
                    if add_similar:
                        weigth = 0.2
                    docname = self.corpus.doc_names[i]
                    if docname in best_matches:
                        best_matches[docname] = best_matches[docname] + (
                            tfidf[i][index] * weigth)
                    else:
                        best_matches[docname] = tfidf[i][index] * weigth

        # Since we need the index for the tfidf_score
        # ATM we are only checking words that are in the corpus
        for qword in query:
            search(self.corpus.features.index(qword))

        if add_similar:
            if self.model_is_loaded:
                potential_similar = self.embedder.get_most_similar(query)
                for i in range(len(potential_similar)):
                    if potential_similar[i] in self.corpus.features:
                        similar.append(potential_similar[i])
            for qword in similar:
                search(self.corpus.features.index(qword), add_similar=True)

        return best_matches, list(set(query + similar))

    def get_processed_query(self, query, threshold, strategy):
        """Preprocess a query in order to keep only corpus's features.

        Parameters
        ----------
        query: str
            The query to process.
        threshold: int
            The minimum similarity threshold.
        strategy: str
            The preprocessing strategy to use.

        Returns
        -------
        array(str)
           An array of query's words filtered of stopwords and flags,
           with only words in the corpus's features.
        """
        res = []
        for word in query:
            if word not in stopwords.words("french") and "--" != word[:2]:
                # Ignore stopwords and flags
                best_index = self.get_corresponsing_feature_index(
                    word, threshold, strategy)
                if best_index is not None:
                    res.append(self.corpus.features[best_index])
        return res

    def get_corresponsing_feature_index(self, word, threshold, strategy):
        """
        Returns the index of `word` in `self.corpus.features`.
        If the `word` is not in it, returns the index of the feature with the
        minimum levenshtein distance.

        Parameters
        ----------
        word: str
            A word of the query.

        Returns
        -------
        int
            An index in `self.corpus.features`.
        """
        feats = self.corpus.features
        try:
            return feats.index(word)
        except ValueError:
            if "spellcheck" == strategy:
                best = self.spellchecker.correction(word)
                if best in feats and is_above_threshold(word, best, threshold):
                    return feats.index(best)
                for c in self.spellchecker.candidates(word):
                    if c in feats and is_above_threshold(word, c, threshold):
                        return feats.index(c)

            elif "embeddings" == strategy:
                if self.model_is_loaded:
                    best = self.embedder.get_corrected_word(word)
                    if best in feats and is_above_threshold(
                            word, best, threshold):
                        return feats.index(best)

            best = None
            mindiff = -1

            for i in range(len(feats)):
                candidate = feats[i]
                dist = distance.levenshtein(word, candidate)
                if (mindiff == -1 or dist <= mindiff) and is_above_threshold(
                        word, candidate, threshold):
                    mindiff = dist
                    best = i
            return best


def is_above_threshold(source, candidate, threshold):
    """Returns whether or not the candidate similarity is above the threshold.

    Parameters
    ----------
        source: str
            The source word.
        candidate: str
            The candidate word.
        threshold: int
            The similarity threshold.

    Returns
    -------
        bool: True if the candidate similarity with the source
                word is abovethe threshold.
    """
    dist = distance.levenshtein(source, candidate)
    sim = 1 - (dist / max(len(source), len(candidate)))
    return sim > threshold / 100
