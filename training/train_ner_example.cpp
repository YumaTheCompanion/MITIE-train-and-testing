/*
    This example shows how to use the MITIE C++ API to train a named_entity_extractor.
*/

#include <mitie/ner_trainer.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

using namespace dlib;
using namespace std;
using namespace mitie;

// ----------------------------------------------------------------------------------------

int main(int argc, char** argv)
{
    /*
        Usage: ./train_ner_example /home/okanb/MITIE/examples/cpp/train_ner/modifications.txt /home/okanb/MITIE/examples/cpp/train_ner/NER-datasets/CONLL2003/train.txt ../../../MITIE-models/english/total_word_feature_extractor.dat; 
    */
    
    if (argc != 4)
    {
        cout << "You must give the path to the modifications.txt file." << endl;
        cout << "You must give the path to the train.txt file." << endl;
        cout << "You must give the path to the MITIE English total_word_feature_extractor.dat file." << endl;
        cout << "So run this program with a command like: " << endl;
        cout << "./train_ner_example ./NER-datasets/CONLL2003/train.txt ../../../MITIE-models/english/total_word_feature_extractor.dat" << endl;
        return 1;
    }
    
    // Here, we load the input text files and the dat files.
    std::ifstream infile_modifications(argv[1]);
    std::ifstream infile(argv[2]); 
    ner_trainer trainer(argv[3]);
    
    // First of all, we want to read the modifications.txt file, because, the ingredients of that file
    // is basically a modification to the "tag" structure of the main input text file.
    // We will put the found tags into a map
    std::map<string, string> mod_map;
    std::string word_to_be_modified, modified_NER_tag;   
    while (infile_modifications >> word_to_be_modified >> modified_NER_tag)
    {
        mod_map.insert(std::pair<std::string, std::string>(word_to_be_modified, modified_NER_tag));
        std::cout << "Got modification: " << word_to_be_modified << " with tag " << modified_NER_tag << std::endl;
    }
    
    bool new_sentence_is_created = false;
    std::string sentence_ending(".");
    std::string sentence_no_tag_incliusion("O");
    std::vector<std::string> *sentence = nullptr;
    std::vector<std::pair<int, std::string>> *entities = nullptr;
    ner_training_instance *sample = nullptr;       
    std::string word, part_of_speech_tag, chunk_tag, NER_tag;    
    sentence = new std::vector<std::string>();  
    entities = new std::vector<std::pair<int, std::string>>();
    //sample = new ner_training_instance(*sentence);
    new_sentence_is_created = true;
    size_t current_line = 0;

    while (infile >> word >> part_of_speech_tag >> chunk_tag >> NER_tag)
    {              
        std::cout << "Reading word: " << word << "\n";
        if(word.compare(sentence_ending) == 0) {         
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
            } else {               
                sentence->push_back(word);
                if(mod_map.find(word.c_str()) != mod_map.end()) {                    
                    entities->emplace_back(sentence->size()-1, mod_map[word.c_str()]);
                    std::cout << "Modified " << word << "'s tag with '" << mod_map[word.c_str()] << "' at line: " << current_line << std::endl;  
                }
            }
        }
        current_line++;
    }

    // The trainer can take advantage of a multi-core CPU.  So set the number of threads
    // equal to the number of processing cores for maximum training speed.
    trainer.set_num_threads(8);
    // This function does the work of training.  Note that it can take a long time to run
    // when using larger training datasets.  So be patient.
    named_entity_extractor ner = trainer.train();

    // Now that training is done we can save the ner object to disk like so.  This will
    // allow you to load the model back in using mitie_load_named_entity_extractor("new_ner_model.dat").
    serialize("new_ner_model.dat") << "mitie::named_entity_extractor" << ner;


    /*
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
     * */
}

// ----------------------------------------------------------------------------------------
