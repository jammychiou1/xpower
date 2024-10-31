#!/usr/bin/env fish
mkdir tmp
for i in (seq 10)
    ./build/debug/rqmul_dbg < ./scripts/testcases/$i.in > ./tmp/out
    if not diff -q ./scripts/testcases/$i.out ./tmp/out
        rm -r tmp
    end
end

rm -r tmp
