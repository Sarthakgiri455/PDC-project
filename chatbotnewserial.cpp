#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <omp.h>
#define MAX_LENGTH 100

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

    for(int i=0;i<vocabulary.size();i++)
    {
        cout<<vocabulary[i]<<" " ;
    }
    cout<<endl;
    return vocabulary;
}


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
    for(int i=0;i<count_vector.size();i++)
    {
        cout<<count_vector[i]<<" ";

    }
    cout<<endl;
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
    double cosine = sum / (norm_v1 * norm_v2);
    cout<<"Cosine similarity is"<<cosine<<endl;
    return cosine;
}
string respond(string text, vector<string> patterns, vector<string>responses) {
    int num_sentences = patterns.size();
    vector<double> scores(num_sentences, 0.0);    
    // #pragma omp parallel for
    for (int i = 0; i < num_sentences; i++) {        
        vector<string>sentences;
        sentences.push_back(text);
        sentences.push_back(patterns[i]);
        vector<string>vocabulary= create_vocabulary(sentences);
        vector<int>countvectorinput= create_count_vector(text, vocabulary);
        vector<int>countvectorpattern= create_count_vector(patterns[i], vocabulary);
        cout<<"COUNT VECTOR PAIRS"<<endl;
        for(int i=0;i<countvectorinput.size();i++)
        {
            cout<<countvectorinput[i]<<" ";
        }
        cout<<endl;
        for(int i=0;i<countvectorinput.size();i++)
        {
            cout<<countvectorpattern[i]<<" ";
        }

        cout<<endl;
        scores[i] = cosine_similarity(countvectorinput, countvectorpattern);
    }
    cout<<endl;
    // #pragma omp barrier
    double maxscore=-1;
    int indexreply=0;
    for(int i=0;i<scores.size();i++)
    {
        cout<<scores[i]<<" "<<endl;
        if(scores[i]>maxscore)
        {
            indexreply=i;
            maxscore=scores[i];
        }
    }
    cout<<"MAX SCORE IS "<<maxscore<<endl;
    if(maxscore==-1)
    {
        cout<<"I could not understand, could you please rephrase"<<endl;
    }
    else{
        cout<<maxscore<<endl;
        cout<<responses[indexreply];
        
    }
    
    // for(int i=0)
    // if (scores[max_score_index] == 0) {
    //     return "I'm sorry, I don't understand what you're asking. Can you please rephrase your question?";
    // } else {
    //     return patterns[max_score_index];
    // }
}
int main()
{   
    vector<string>patterns={"hello", "what is your name", "what chatbot are you","i am having frequent urination","i am having blurry vision","i have tightness in chest","medicines for diabetes","i am having anxiety"};
    
   vector<string>responses={"hi,good to see you","I am a medibot","I am a medical assistant chatbot","You may be suffering from diabetes, take some insulin","That is a diabetes symptom, may be you should consult doctor","that is asthma symptom..take some pantaprzol","you should take insulin","you may be suffering from depression"};

    // cout<<patterns.size();
    // cout<<responses.size();
    string userinput;
   while(true)
   {
        cout<<"Input :";

        double t1=omp_get_wtime();
        
        getline(cin,userinput);
        if(userinput=="exit" || userinput=="end")
        {
            cout<<"Thank you for your time"<<endl;
            break;
        }       
           respond(userinput, patterns, responses);
           cout<<endl;

           double t2= omp_get_wtime();
           cout<<"TIME FOR RESPONSE IS"<<endl;
           cout<<t2-t1<<endl;
           cout<<endl;               

   }

}

