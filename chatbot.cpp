#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <omp.h>

using namespace std;

// function to split a string into words
vector<string> split_string(string str) {
    vector<string> words;
    string word = "";
    for (char c : str) {
        if (c == ' ') {
            if (word != "") {
                words.push_back(word);
                word = "";
            }
        } else {
            word += c;
        }
    }
    if (word != "") {
        words.push_back(word);
    }
    return words;
}

// function to create the vocabulary
vector<string> create_vocabulary(vector<string> sentences) {
    vector<string> vocabulary;
    for (string sentence : sentences) {
        vector<string> words = split_string(sentence);
        for (string word : words) {
            if (find(vocabulary.begin(), vocabulary.end(), word) == vocabulary.end()) {
                vocabulary.push_back(word);
            }
        }
    }
    sort(vocabulary.begin(), vocabulary.end());
    return vocabulary;
}

// function to create the count vector for a sentence
vector<int> create_count_vector(string sentence, vector<string> vocabulary) {
    vector<int> count_vector(vocabulary.size(), 0);
    vector<string> words = split_string(sentence);
    for (string word : words) {
        auto it = find(vocabulary.begin(), vocabulary.end(), word);
        if (it != vocabulary.end()) {
            int index = it - vocabulary.begin();
            count_vector[index]++;
        }
    }
    return count_vector;
}


double cosine_similarity(vector<int> v1, vector<int> v2) {
    int dot_product = 0;
    int sum=0;

    #pragma omp parallel for reduction(+:sum)

        for (int i = 0; i < v1.size(); i++) {
            sum += v1[i] * v2[i];
        }
    double norm_v1 = 0;
    double norm_v2 = 0;
    for (int i = 0; i < v1.size(); i++) {
        norm_v1 += pow(v1[i], 2);
        norm_v2 += pow(v2[i], 2);
    }
    norm_v1 = sqrt(norm_v1);
    norm_v2 = sqrt(norm_v2);
    double cosine = dot_product / (norm_v1 * norm_v2);
    return cosine;
}
string respond(string text, vector<string> patterns) {
    int num_sentences = patterns.size();
    vector<double> scores(num_sentences, 0.0);    
    #pragma omp parallel for
    for (int i = 0; i < num_sentences; i++) {        
        vector<string>sentences;
        sentences.push_back(text);
        sentences.push_back(patterns[i]);
        vector<string>vocabulary= create_vocabulary(sentences);
        vector<int>countvectorinput= create_count_vector(text, vocabulary);
        vector<int>countvectorpattern= create_count_vector(patterns[i], vocabulary);
        scores[i] = cosine_similarity(countvectorinput, countvectorpattern);
    }
    for(int i=0;i<scores.size();i++)
    {
        cout<<scores[i]<<" "<<endl;
    }
    int max_score_index = max_element(scores.begin(), scores.end()) - scores.begin();
    if (scores[max_score_index] == 0) {
        return "I'm sorry, I don't understand what you're asking. Can you please rephrase your question?";
    } else {
        return patterns[max_score_index];
    }
}
int main()
{   
    vector<string>patterns={"hello", "what is your name", "what chatbot are you","i am having frequent urination","i am having blurry vision","i have tightness in chest","medicines for diabetes","Having Anxiety"};
    
   vector<string>responses={"hi,good to see you","I am a medibot","I am a medical assistant chatbot","You may be suffering from diabetes, take some insulin","That is a diabetes symptom, may be you should consult doctor","that is asthma symptom..take some pantaprzol","you should take insulin","you may be suffering from depression"};

    // cout<<patterns.size();
    // cout<<responses.size();
    string userinput;
   while(true)
   {
        cout<<"Input :";
        
        cin>>userinput;
        if(userinput=="exit" || userinput=="end")
        {
            cout<<"Thank you for your time"<<endl;
            break;
        }
        cout<<respond(userinput, patterns)<<endl;

   }

}

