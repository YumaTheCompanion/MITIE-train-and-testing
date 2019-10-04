#!/bin/bash

make;
[ $? -eq 0 ]  || exit 1
./train_ner_example ../../../MITIE-models/english/total_word_feature_extractor.dat;
[ $? -eq 0 ]  || exit 1
cat sample_text.txt | ../../../ner_stream new_ner_model.dat
[ $? -eq 0 ]  || exit 1

echo "\n\nHere is the original result\n\n"

cat sample_text.txt | ../../../ner_stream ../../../MITIE-models/english/ner_model.dat;
