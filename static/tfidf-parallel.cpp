#include <bits/stdc++.h>
#include <omp.h>

using namespace std;

// Regular expression for tokenization
const regex re(R"([\s|,]+)");
const regex sre(R"([\\.][\s]+)");

vector<string> tokenize(const string str, const regex re)
{
    sregex_token_iterator it{ str.begin(), str.end(), re, -1 };
    vector<std::string> tokenized{ it, {} };
 
    // Additional check to remove empty strings
    tokenized.erase(remove_if(tokenized.begin(),
                            tokenized.end(),
                            [](string const& s) {
                            return s.size() == 0;
                            }), 
                    tokenized.end()
    );
 
    return tokenized;
}
 
void sanitize(string &token){
    if(!iswalnum(token[token.size() - 1])) {
        token.pop_back();
    }
    transform(token.begin(), token.end(), token.begin(), ::tolower);
}

map<string, int> termFrequency(vector<string> &tokens){
    map<string, int> tf;
    #pragma omp parallel for
    for(string token: tokens){
        sanitize(token);
        ++tf[token];
    }
    return tf;
}

double calculateScore(string sentence, map<string, int> &tf, map<string, int> &idf){
    vector<string> words = tokenize(sentence, re);
    double score = 0;
    #pragma omp parallel for reduction(+: score)
    for(auto &word: words){
        if(!idf[word]) continue;
        score += 100.0 * tf[word] / idf[word];
    }
    return score;
}


int main(){
    omp_set_num_threads(6);

    cout << "-----------------------------------" << '\n';
    cout << "-    Parallel Summarization       -" << '\n';
    cout << "-----------------------------------\n" << '\n';

    // Start time
    double t1 = omp_get_wtime();

    string fname="input.txt";

    // Reading the input file
    fstream fin(fname);
    string text="";
    string line;
    while(getline(fin, line)){
        text += line + ' ';
    }
    fin.close();
       
    // Word Tokenization 
    vector<string> tokenized = tokenize(text, re);
    
    // Sentence Tokenization 
    vector<string> sentences = tokenize(text, sre);

    // Finding term frequencies
    map<string, int> tf = termFrequency(tokenized);
    
    // Generating inverse document frequency
    fstream model("model.txt");

    map<string, int> idf = tf;

    string word;
    int frequency;

    while(model >> word >> frequency){
        idf[word] += frequency;
    }
    
    model.close();

    double ratio = 1.0/3;
    int summarySize = ceil(ratio * sentences.size()); 
    
    priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> pq;

    // cout << sentences.size() << '\n';
    // Ranking sentences
    #pragma omp parallel for
    for(int i=0; i<sentences.size(); ++i){
        double score = calculateScore(sentences[i], tf, idf);
        #pragma omp critical
        {
            pq.push({score, i});
            if(pq.size() > summarySize) 
                pq.pop();
        }
    }

    fstream fout("output.txt");
    while(!pq.empty()){
        int index = pq.top().second;
        pq.pop();
        fout << sentences[index] << '\n';
    }
    fout.close();

    // Updating the inverse document frequency
    // #pragma omp parallel for 
    fstream updateModel("model.txt");
    for(auto &mod: idf){
        updateModel << mod.first << ' ' << mod.second << '\n';
    }
    updateModel.close();

    double t2 = omp_get_wtime();

    cout << "- Summarization Completed -" << '\n';
    cout << "- Time Taken : " << t2-t1 << " s -\n";

    return 0;
}