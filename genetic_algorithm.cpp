#include <iostream>
#include <vector>
#include <time.h>
#include <string.h>
#include <cmath>
#include <unordered_set>
#include <algorithm>

const int kDefaultClusterNumber = 20;

struct Node {
    int score;
    std::vector<int> queen;
    bool operator < (const Node& other) const {
        return score < other.score;
    }
};

// 随机产生一个解
void get_init(Node& ori, int N) {
    int num = 0;
    bool arr[N];
    memset(arr, 0, sizeof(arr));
    ori.queen.resize(N);
    for (int i = 0; i < N; i++) {
        ori.queen[i] = 0;
    }
    ori.score = 0;

    for (int i = 0; i < N; i++) {
        num = rand() % N;
        while (arr[num]) {
            num = rand() % N;
        }
        arr[num] = true;
        ori.queen[i] = num;
    }
}

int score(Node& ori, int N) {
    std::unordered_set<int> leftdiagset;
    std::unordered_set<int> rightdigaset;
    for (int i = 0; i < N; i++) {
        leftdiagset.insert(ori.queen[i] - i);
        rightdigaset.insert(ori.queen[i] + i);
    }
    int score = N * 2 - (leftdiagset.size() + rightdigaset.size());
    if (score <= 1) {
        return score;
    } else  {
        return score*(score-1)/2;
    }
}

int choose_one(std::vector<Node>& scores, int choose_size) {
    int fitness = 0x7fffffff;
    int index = 0;
    int max_size = scores.size();
    for (int i = 0; i < choose_size; ++i) {
        int randomIndex = rand() % max_size;
        const Node& tmp = scores[randomIndex];
        if (fitness > tmp.score) {
            fitness = tmp.score;
            index = randomIndex;
        }
    }

    return index;
}

bool exchange_opt(std::vector<Node>& result_vec, int left, int right, int N) {
    // 随机交换
    int mid = (N >> 1);
    int start = rand() % mid;
    int end = mid + (rand() % mid);
    std::unordered_set<int> dul;
    Node new_one;
    new_one.queen.resize(N);
    for (int i = 0; i < N; i++) {
        new_one.queen[i] = -1;
    }
   
    for (int i = start; i <= end; i++) {
        new_one.queen[i] = result_vec[left].queen[i];
        dul.insert(new_one.queen[i]);
    }

    int j = 0;
    for (int i = 0; i < N; i++) {
        if (new_one.queen[i] != -1) {
            continue;
        }
        while (dul.count(result_vec[right].queen[j])) {
            j++;
        }
        new_one.queen[i] = result_vec[right].queen[j];
        dul.insert(new_one.queen[i]);
    }
    new_one.score = score(new_one, N);
    if (new_one.score == 0) {
        return true;
    }

    result_vec.push_back(new_one);
    return false;
}

bool mutation(std::vector<Node>& result_vec, int index_num, int N) {
    int maxSize = result_vec.size();
    int index_bit = rand() % N;
    int mutation_from = rand() % N;
    int mutation_to = rand() % N;
    std::swap(result_vec[index_num].queen[mutation_from], result_vec[index_num].queen[mutation_to]);
    result_vec[index_num].score = score(result_vec[index_num], N);   
    if (result_vec[index_num].score == 0) {
        return true;
    }
    return false;
}

bool genetic_algorithm(int N) {
    int cluster_number = 0;
    double pro_abnormal = 0.2;
    int choose_size = 5;
    if (N > kDefaultClusterNumber) {
        cluster_number = N;
    } else {
        cluster_number = (N*(N-1)/2) > kDefaultClusterNumber ? kDefaultClusterNumber: 2 * N;
    }

    std::vector<Node> cluster_vec;
    Node ori;
    for (int i = 0; i < cluster_number; i++) {
        get_init(ori, N);
        cluster_vec.push_back(ori);
    }
    
    for (int i = 0; i < cluster_vec.size(); i++) {
        cluster_vec[i].score = score(cluster_vec[i], N);
        if (cluster_vec[i].score == 0) {
            return true;
        }
    }
    int loopCnt = 0;
    while (true) {
        if (loopCnt != 0 && loopCnt % 100000 == 0) {
            std::cout << "loopCnt = " << loopCnt << std::endl;
        }
        loopCnt++;
        int presize = cluster_vec.size()/4;
        for (int i = 0; i < presize; i++) {
            int left = choose_one(cluster_vec, choose_size);
            int right = choose_one(cluster_vec, choose_size);
            if (exchange_opt(cluster_vec, left, right, N)) {
                return true;
            }
        }
       
        // 根据概率查看是否变异
        for (int i = 0; i < presize; i++) {
            double pro = double(rand() % 100 + 1) / 100;
            if (pro_abnormal >= pro) {
                if (mutation(cluster_vec, i, N)) {
                    return true; 
                }
            }
        }
        std::sort(cluster_vec.begin(), cluster_vec.end());
        while (cluster_vec.size() > cluster_number) {
            cluster_vec.pop_back();
        }
    }
    return false;
}

int main(int argc, const char* argv[]) {
    std::vector<int> testN = {4, 8, 16, 32, 64, 128, 256, 512};
    srand((unsigned(time(NULL))));
    clock_t start, end;
    // 遗传算法
    for (int index = 0; index < testN.size(); index++) {
        int N = testN[index];
        int successCnt = 0;
        int failCnt = 0;
        int execTime = 0;
        int totalExecCnt = 1;
       
        start = clock();
        bool result = genetic_algorithm(N);
        end = clock();
        execTime += (end - start) / 1000;
        std::cout << "遗传算法 N = " << N << " 平均执行时间" << execTime / totalExecCnt << "ms" << std::endl;
    }
    return 0;
}