##!/bin/bash

device="/dev/hexdump"
test_dir="test_files"
golden_hex_res_dir="hex_results"
hex_result="/tmp/loop"

if [[ ! -e "$device" ]]; then
    echo "$device does not exist"
    exit 1
fi

if [[ ! -d "$test_dir" || ! -d "$golden_hex_res_dir" ]]; then
  echo "Error: [$test_dir , $golden_hex_res_dir] One or both directories do not exist."
  exit 1
fi

files=("$test_dir"/*)
file_count=${#files[@]}

for ((i=1; i <= $file_count; i++)); do
  file="${files[$i-1]}"
  output_file="$golden_hex_res_dir/${file##*/}.hex"
  cat $file > $device
  hexdump $file > $output_file
  diff $output_file $hex_result > /dev/null 2>&1
  if [[ $? -eq 0 ]]; then
    echo "No diff found for $file"
  else
    echo "Diff found for $file"
  fi
  rm -rf $hex_result

done
