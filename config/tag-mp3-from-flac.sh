#!/bin/sh
#
# usage: tag-mp3-from-flac.sh "/path/to/original/file.flac" "/path/to/new/file_to_tag.mp3"
#

if [ $# -ne 2 ]; then
  echo "Wrong number of parameters!" 1>&2
  exit 0;
fi

# tr is for multi artist tags, and the last sed is to remove the - from the end of the folder name
ARTIST=$(metaflac "$1" --show-tag=ARTIST | sed s/.*=//g | tr '\n' '-' | sed s/[-]$//)
TITLE=$(metaflac "$1" --show-tag=TITLE | sed s/.*=//g | tr '\/' '--')
ALBUM=$(metaflac "$1" --show-tag=ALBUM | sed s/.*=//g | tr '\/' '--')
GENRE=$(metaflac "$1" --show-tag=GENRE | sed s/.*=//g)
# second sed is for removing the /x (total tracks) from filename and tag
TRACKNUMBER=$(metaflac "$1" --show-tag=TRACKNUMBER | sed s/.*=//g | sed s/[/].*//)
DATE=$(metaflac "$1" --show-tag=DATE | sed s/.*=//g)

id3 -t "$TITLE" -T "${TRACKNUMBER:-0}" -a "$ARTIST" -A "$ALBUM" -y "$DATE" -g "${GENRE:-12}" "$2"

# extract folder from filename
FOLDER=$(echo "$2" | sed 's|\(.*\)/.*|\1|')
# extract only filename extension
EXTENSION=$(echo $2 | sed -e 's/.*\.//')

ARTIST_DIR="$FOLDER/$ARTIST";
ALBUM_DIR="$FOLDER/$ARTIST/$DATE $ALBUM";

MOVE_FILENAME="$ALBUM_DIR/$TRACKNUMBER - $TITLE.$EXTENSION";

if [ ! -d "$ARTIST_DIR" ]; then
  mkdir "$ARTIST_DIR";
fi

if [ ! -d "$ALBUM_DIR" ]; then
  mkdir "$ALBUM_DIR";
fi

mv "$2" "$MOVE_FILENAME";
