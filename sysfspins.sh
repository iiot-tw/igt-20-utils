if [ "$(which igt$1)" == "" ]
then
echo File igt$1 not found
exit 1
fi

cat `which igt$1` | grep ')\s*PIN=' | sed 's/)\s*PIN=/:/g' | sed 's/\s//g' | sed 's/;//g'
