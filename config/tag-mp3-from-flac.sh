#!/bin/sh
#
# usage: tag-mp3-from-flac.sh "/path/to/original/file.flac" "/path/to/new/file_to_tag.mp3"
#

if [ $# -ne 2 ]; then
  echo "Wrong number of parameters!" 1>&2
  exit 0;
fi

PARSE_METAFLAC_ARG="s/.*=//g"
REPLACE_ILLEGAL_CHARS_ARG="s/[^ A-Za-z0-9._-]/_/g"

# tr is for multi artist tags, and the last sed is to remove the - from the end of the folder name
ARTIST=$(metaflac "$1" --show-tag=ARTIST | sed "$PARSE_METAFLAC_ARG" | tr '\n' '-' | sed s/[-]$//)
TITLE=$(metaflac "$1" --show-tag=TITLE | sed "$PARSE_METAFLAC_ARG")
ALBUM=$(metaflac "$1" --show-tag=ALBUM | sed "$PARSE_METAFLAC_ARG")
GENRE=$(metaflac "$1" --show-tag=GENRE | sed "$PARSE_METAFLAC_ARG")
TRACKNUMBER=$(metaflac "$1" --show-tag=TRACKNUMBER | sed "$PARSE_METAFLAC_ARG")
DATE=$(metaflac "$1" --show-tag=DATE | sed "$PARSE_METAFLAC_ARG")

id3 -t "$TITLE" -T "${TRACKNUMBER:-0}" -a "$ARTIST" -A "$ALBUM" -y "$DATE" -g "${GENRE:-12}" "$2"

# extract folder from filename
FOLDER=$(echo "$2" | sed "s|\(.*\)/.*|\1|")
# extract only filename extension
EXTENSION=$(echo $2 | sed -e "s/.*\.//")

#ARTIST_DIR="$FOLDER/$ARTIST";
#ALBUM_DIR="$FOLDER/$ARTIST/$DATE $ALBUM";
ARTIST_DIR=$(echo "$ARTIST" | sed "$REPLACE_ILLEGAL_CHARS_ARG")
ALBUM_DIR=$(echo "$DATE $ALBUM" | sed "$REPLACE_ILLEGAL_CHARS_ARG");
FILENAME=$(echo "$TRACKNUMBER - $TITLE.$EXTENSION" | sed "$REPLACE_ILLEGAL_CHARS_ARG");

MOVE_FILENAME="$FOLDER/$ARTIST_DIR/$ALBUM_DIR/$FILENAME";

if [ ! -d "$FOLDER/$ARTIST_DIR" ]; then
  mkdir "$FOLDER/$ARTIST_DIR";
fi

if [ ! -d "$FOLDER/$ARTIST_DIR/$ALBUM_DIR" ]; then
  mkdir "$FOLDER/$ARTIST_DIR/$ALBUM_DIR";
fi

mv "$2" "$MOVE_FILENAME";
