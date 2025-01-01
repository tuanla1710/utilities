mkfifo /tmp/myfifo1
mkfifo /tmp/myfifo2

gcc writer.c -o writer
gcc reader.c -o reader