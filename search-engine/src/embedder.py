import re
from gensim.models import KeyedVectors
from nltk.corpus import stopwords
import distance

# @see http://norvig.com/spell-correct.html for the correction algorithm.
# @see https://fauconnier.github.io/ for the model.

MODEL_PATH = "models/frWac_postag_no_phrase_1000_skip_cut100.bin"
SUFFIXES = ["n", "v"]
SEP = "_"


class Embedder():
    def __init__(self) -> None:
        self.model = KeyedVectors.load_word2vec_format(MODEL_PATH,
                                                       binary=True,
                                                       unicode_errors="ignore")
        words = self.model.index_to_key
        w_rank = {}
        for i, word in enumerate(words):
            w_rank[word] = i
        self.stop_words = stopwords.words('french')
        self.WORDS = w_rank

    def __P(self, word):
        """Probability of `word`."""
        # returns 0 if the word isn't in the dictionary
        return -self.WORDS.get(word, 0)

    def __candidates(self, word):
        """Generate possible spelling corrections for word."""
        return (self.__known([word]) or self.__known(self.__edit_dist_1(word))
                or self.__known(self.__edit_dist_2(word)) or [word])

    def __known(self, words):
        """The subset of `words` that appear in the dictionary of WORDS."""
        return set(w for w in words if w in self.WORDS)

    def __edit_dist_1(self, word):
        """All edits that are one edit away from `word`."""
        letters = 'abcdefghijklmnopqrstuvwxyz'
        splits = [(word[:i], word[i:]) for i in range(len(word) + 1)]
        deletes = [L + R[1:] for L, R in splits if R]
        transposes = [L + R[1] + R[0] + R[2:] for L, R in splits if len(R) > 1]
        replaces = [L + c + R[1:] for L, R in splits if R for c in letters]
        inserts = [L + c + R for L, R in splits for c in letters]
        return set(deletes + transposes + replaces + inserts)

    def __edit_dist_2(self, word):
        """All edits that are two edits away from `word`."""
        return (e2 for e1 in self.__edit_dist_1(word)
                for e2 in self.__edit_dist_1(e1))

    def get_corrected_word(self, word):
        """Corrects a misspelled word using a model.

        Parameters
        ----------
        word: str  
            The word to correct.

        Returns
        -------
        str
            The most plausible correction of the word.
        """
        res = []
        for s in SUFFIXES:
            res.append(
                max(self.__candidates(word + SEP + s),
                    key=self.__P).replace(SEP + s, ''))
        return max(res, key=self.__P)

    def __vsort(self, lst, reverse=True):
        """ Sorts a list of tuples (k, v) using the tuple left value v.
        """
        return (sorted(lst, key=lambda x: x[1], reverse=reverse))

    def __to_klist(self, tlst):
        """ Transfroms a list of tuple (k, v) into the list of k.
        """
        return list(dict(tlst).keys())

    def __split_on_SEP(self, lst):
        """ Splits elements the given list on SEP, and keep the first part.
        """
        return list(dict.fromkeys([r.split(SEP)[0] for r in lst]))

    def get_most_similar(self, query):
        """ Gets the most similar words for each word of the given query.
        
        Parameters
        ----------
        query: list(str)  
            The query for which we are looking for similars.

        Returns
        -------
        list(str):  
            The most similar words that are in the model.
        """
        res = []
        for w in query:
            for s in SUFFIXES:
                try:
                    for sim in self.model.similar_by_word(w + SEP + s):
                        # Neither adverbs nor pronouns in the similar words.
                        if "_adv" not in sim[0] and "_pro" not in sim[0]:
                            res.append(sim)
                except:
                    # Exception when there is no match
                    pass
            # compute levensthein distances
            sim = [(i, distance.levenshtein(w, i))
                   for i in self.__split_on_SEP(
                       self.__to_klist(self.__vsort(res)))]

            # order by levensthein distances, remove source word & stop_words
            sim = [
                s for s in self.__to_klist(self.__vsort(sim, reverse=True))
                if ((s != w) and (s not in self.stop_words))
            ]

        return sim
