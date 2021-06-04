#! /usr/bin/env bash
#
# Script from:
#
# 	https://github.com/CatalaLang/catala/blob/master/syntax_highlighting/en/setup_vscode.sh
#

FILE=~/.vscode/extensions/honu
SCRIPT=`realpath $0`
SCRIPT_PATH=`dirname $SCRIPT`

if [ -L "$FILE" ]; then
	rm "$FILE"
fi

echo "Add symbolic link"
ln -s -f $SCRIPT_PATH/vscode "$FILE"
