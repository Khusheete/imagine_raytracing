#!/usr/bin/env fish

for f in $(ls)
    if string match -rg ".+\.ppm" $f
        echo "Converting "$f" ..."
        set image $(string replace ".ppm" "" $f)
        set dest "$image.png"
        magick "$f" "$dest"
        and rm "$f"
    end
end
