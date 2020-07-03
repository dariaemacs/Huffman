# Huffman algorithm
This is simple realisation of Huffman code for beginners. It's used for lossless data compression.
It works well for text files if file size is greater than 1000 bytes. The compression reaches 50% for big files.
## This histogram shows bad compression for small files:
![Alt text](https://github.com/dariaemacs/Huffman/blob/master/png/small_files_compression.png?raw=true "Optional Title")
## How to start
Compress:   $ make && ./main -c texts/text_hamlet.txt


Decompress: $ make && ./main -d texts/text_hamlet.txt.hff
