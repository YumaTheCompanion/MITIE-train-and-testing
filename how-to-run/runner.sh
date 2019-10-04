#!/bin/bash

make;
[ $? -eq 0 ]  || exit 1
./train_ner_example /home/okanb/MITIE/examples/cpp/train_ner/modifications.txt /home/okanb/MITIE/examples/cpp/train_ner/NER-datasets/CONLL2003/train.txt ../../../MITIE-models/english/total_word_feature_extractor.dat > train_output_results.txt;
[ $? -eq 0 ]  || exit 1
cat sample_text.txt | ../../../ner_stream new_ner_model.dat > new_ner_model_analysis.txt
[ $? -eq 0 ]  || exit 1

cat sample_text.txt | ../../../ner_stream new_ner_model_0001.dat > new_ner_model_analysis_0001.txt

echo "\n\nvs original data\n\n"

cat sample_text.txt | ../../../ner_stream ../../../MITIE-models/english/ner_model.dat;
[ $? -eq 0 ]  || exit 1
