from os import read
from similar import get_most_similar, vsort
from sklearn.feature_extraction.text import TfidfVectorizer
from sklearn.metrics.pairwise import linear_kernel
from nltk.stem.snowball import FrenchStemmer
from nltk import word_tokenize          
from nltk.corpus import stopwords
from corpus import read_docs

ignore_tokens = [',', '.', ';', ':', '"', '``', "''", '`', ]
stop_words = set(stopwords.words('french'))

class LemmaTokenizer:
    ignore_tokens = [',', '.', ';', ':', '"', '``', "''", '`']  
    def __init__(self):
        self.wnl = FrenchStemmer()
    def __call__(self, doc):
        return [self.wnl.stem(t) for t in word_tokenize(doc) if t not in self.ignore_tokens]

class Engine:
    def __init__(self, path):
        self.documents = []
        self.listnames = []
        self.documents, self.listnames = read_docs(path)
        tokenizer = LemmaTokenizer()
        token_stop = tokenizer(' '.join(stop_words))
        self.vectorizer = TfidfVectorizer(stop_words=token_stop, tokenizer=tokenizer)

    def scores_from_query(self, query) :
        doc_vectors = self.vectorizer.fit_transform([query] + self.documents)
        sim = linear_kernel(doc_vectors[0:1], doc_vectors).flatten()
        return [item.item() for item in sim[1:]]

    def get_main_results(self, searched_word):
        result = []
        # searched_word = ' '.join([s for s in searched_word if s not in stop_words])
        scores = self.scores_from_query(searched_word)
        for i in range(len(scores)) :
            result.append((self.listnames[i], scores[i]))
        return vsort(result)

    def get_additional_results(self, searched_word) :
        result = {}
        similar = get_most_similar(searched_word)
        # similar.append(searched_word)
        try :
            similar.remove(searched_word)
        except :
            pass

        for word in similar :
            scores = self.scores_from_query(word)

            for i in range(len(scores)) :
                # if listnames[i] not in result.keys():
                if scores[i] != 0.0 :
                    if self.listnames[i] in result.keys() and result[self.listnames[i]][0] < scores[i] :
                        continue
                    else :
                        result[self.listnames[i]] = (scores[i], word) 

        # dict to list to keep the same result type
        return [(k, v) for k, v in result.items()]