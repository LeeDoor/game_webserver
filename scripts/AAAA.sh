for file in /usr/local/pgsql/bin/*; do
    filename=$(basename "$file")
    aliasname="${filename%.*}"
    alias "$aliasname"="$file"
done