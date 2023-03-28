#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <omp.h>

using namespace std;

// CountVectorizer function to create vocabulary and vectorize sentences
vector<vector<int>> CountVectorizer(vector<string> sentences, unordered_map<string, int>& vocab) {
    vector<vector<int>> vectors;

    // Create vocabulary
    int count = 0;
    for (auto& sentence : sentences) {
        for (auto& word : sentence) {
            if (vocab.find(word) == vocab.end()) {
                vocab[word] = count;
                count++;
            }
        }
    }

    // Vectorize sentences
    for (auto& sentence : sentences) {
        vector<int> vector(vocab.size(), 0);
        for (auto& word : sentence) {
            vector[vocab[word]]++;
        }
        vectors.push_back(vector);
    }

    return vectors;
}

// Compute cosine similarity between two vectors
double CosineSimilarity(vector<int>& v1, vector<int>& v2) {
    double dot = 0.0, mag1 = 0.0, mag2 = 0.0;
    #pragma omp parallel for reduction(+:dot, mag1, mag2)
    for (int i = 0; i < v1.size(); i++) {
        dot += v1[i] * v2[i];
        mag1 += v1[i] * v1[i];
        mag2 += v2[i] * v2[i];
    }
    mag1 = sqrt(mag1);
    mag2 = sqrt(mag2);
    return dot / (mag1 * mag2);
}

int main() {
    // Dataset
    vector<string> titles = {"sick", "happy", "angry", "neutral"};
   

   vector<vector<string>>
    vector<vector<int>> vectors;
    unordered_map<string, int> vocab;

    // Vectorize sentences
    vectors = CountVectorizer(sentences, vocab);

    // User input
    string input;
    while (true) {
        cout << "Input: ";
        getline(cin, input);

        if (input == "exit") {
            cout << "Response: Exiting....." << endl;
            break;
        }

        // Vectorize input
        vector<string> input_sentence;
        for (auto& word : input) {
            input_sentence.push_back(word);
        }
        vector<int> input_vector(vocab.size(), 0);
        for (auto& word : input_sentence) {
            if (vocab.find(word) != vocab.end()) {
                input_vector[vocab[word]]++;
            }
        }

        // Compute cosine similarity
        vector<double> scores;
        #pragma omp parallel for
        for (int i = 0; i < vectors.size(); i++) {
            double score = CosineSimilarity(vectors[i], input_vector);
            scores.push_back(score);
        }

        // Find index with max score
        int max_score_index = distance(scores.begin(), max_element(scores.begin(), scores.end()));

        // Output response
        if (scores[max_score_index] == 0) {
            cout << "Response: I'm sorry, I don't understand what you're asking. Can you please rephrase your question?" << endl;
        }
        else {
            cout << "Response: " << titles[max_score_index] << endl;

        }
       