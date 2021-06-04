# Snake_search 🐍 ![tag][tag]

A search engine for text corpus written in Python.

![snake_search-v2-demo](/uploads/a96f6190557381b1ab6fd661d25bfe70/snake_search-v2-demo1080.mp4)

The project report is available [here](https://gitlab.com/EmileRolley/search-engine/-/blob/master/doc/pdf/report.pdf).

## Dependencies

Dependencies are listed in *./requirements.txt* and could be install with:

```console
pip install -r requirements.txt
```

## How to..

### .. use `snake search`

The program could be launched with:

```console
> ./src/main.py
```
From there simply run the `help` command for more informations.

### .. use a model

#### Download models

##### Word embedding model for the spelling correction

In order to use the `embedding` strategy, you need to download
the [model](https://s3.us-east-2.amazonaws.com/embeddings.net/embeddings/frWac_postag_no_phrase_1000_skip_cut100.bin)
into the folder `models/`.

[tag]: https://img.shields.io/badge/tag-v2.0-blue
