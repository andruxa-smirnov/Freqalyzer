source defines

echo -e "\033[4;34mExecuting $OUTPUT_NAME\n\033[0m"
export LD_LIBRARY_PATH=../lib
./$OUTPUT_NAME
unset LD_LIBRARY_PATH
echo -e "\033[4;34mDone\n\033[0m"
