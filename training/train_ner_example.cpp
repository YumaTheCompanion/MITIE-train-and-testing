/*
    This example shows how to use the MITIE C++ API to train a named_entity_extractor.
*/

#include <mitie/ner_trainer.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace dlib;
using namespace std;
using namespace mitie;

// ----------------------------------------------------------------------------------------

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        cout << "You must give the path to the MITIE English total_word_feature_extractor.dat file." << endl;
        cout << "So run this program with a command like: " << endl;
        cout << "./train_ner_example ../../../MITIE-models/english/total_word_feature_extractor.dat" << endl;
        return 1;
    }
    ner_trainer trainer(argv[1]);
    // Don't forget to add the training data.  Here we have only two examples, but for real
    // uses you need to have thousands.  
   
    bool new_sentence_is_created = false;
    std::string sentence_ending(".");
    std::string sentence_no_tag_incliusion("O");
    std::vector<std::string> *sentence = nullptr;
    std::vector<std::pair<int, std::string>> *entities = nullptr;
    ner_training_instance *sample = nullptr;
    std::ifstream infile("/home/okanb/MITIE/examples/cpp/train_ner/NER-datasets/CONLL2003/train.txt");
    std::string word, part_of_speech_tag, chunk_tag, NER_tag;
    
    sentence = new std::vector<std::string>();  
    entities = new std::vector<std::pair<int, std::string>>();
    //sample = new ner_training_instance(*sentence);
    new_sentence_is_created = true;

    while (infile >> word >> part_of_speech_tag >> chunk_tag >> NER_tag)
    {
        std::cout << word << ", " << NER_tag << std::endl;        
        if(word.compare(sentence_ending) == 0) {
            //std::cout << "asdasd" << std::endl;
            if(new_sentence_is_created == true) {
                sentence->push_back(word);
                sample = new ner_training_instance(*sentence);                
                for(int i = 0; i < entities->size(); i++) {                    
                    sample->add_entity(entities->at(i).first, 1, entities->at(i).second.c_str());
                }
                trainer.add(*sample);
            }
            sentence = new std::vector<std::string>();  
            entities = new std::vector<std::pair<int, std::string>>();
            //sample = new ner_training_instance(*sentence);
            new_sentence_is_created = true;
        } else {            
            if(NER_tag.compare(sentence_no_tag_incliusion) != 0) {                
                sentence->push_back(word);
                entities->emplace_back(sentence->size()-1, NER_tag);
                
                //std::cout << "asdasd" << sentence->size() << std::endl;
                //sample->add_entity(sentence->size()-1, 1, NER_tag.c_str());
            } else {
                sentence->push_back(word);
            }
        }
        
    }

    // The trainer can take advantage of a multi-core CPU.  So set the number of threads
    // equal to the number of processing cores for maximum training speed.
    trainer.set_num_threads(4);
    // This function does the work of training.  Note that it can take a long time to run
    // when using larger training datasets.  So be patient.
    named_entity_extractor ner = trainer.train();

    // Now that training is done we can save the ner object to disk like so.  This will
    // allow you to load the model back in using mitie_load_named_entity_extractor("new_ner_model.dat").
    serialize("new_ner_model.dat") << "mitie::named_entity_extractor" << ner;


    // But now let's try out the ner object.  It was only trained on a small dataset but it
    // has still learned a little.  So let's give it a whirl.  But first, print a list of
    // possible tags.  In this case, it is just "person" and "org".
    const std::vector<string> tagstr = ner.get_tag_name_strings();
    cout << "The tagger supports "<< tagstr.size() <<" tags:" << endl;
    for (unsigned int i = 0; i < tagstr.size(); ++i)
        cout << "   " << tagstr[i] << endl;

    // Now let's make up a test sentence
    std::vector<std::string> sentenceF;
    sentenceF.push_back("I");
    sentenceF.push_back("met");
    sentenceF.push_back("with");
    sentenceF.push_back("John");
    sentenceF.push_back("Becker");
    sentenceF.push_back("at");
    sentenceF.push_back("HBU");
    sentenceF.push_back(".");

    // Then ask our new ner object to detect the entities.
    std::vector<pair<unsigned long, unsigned long> > chunks;
    std::vector<unsigned long> chunk_tags;
    ner(sentenceF, chunks, chunk_tags);
    // Happily, it found the correct answers, "John Becker" and "HBU" in this case which we
    // print out below.
    cout << "\nNumber of named entities detected: " << chunks.size() << endl;
    for (unsigned int i = 0; i < chunks.size(); ++i)
    {
        cout << "   Tag " << chunk_tags[i] << ":" << tagstr[chunk_tags[i]] << ": ";
        // chunks[i] defines a half open range in sentenceF that contains the entity.
        for (unsigned long j = chunks[i].first; j < chunks[i].second; ++j)
            cout << sentenceF[j] << " ";
        cout << endl;
    }
}

// ----------------------------------------------------------------------------------------
