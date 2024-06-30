#include <iostream>
#include <vector>
#include <time.h>
#include <string.h>
#include <queue>
#include <unordered_set>
#include <cmath>

// 随机产生一个解
void get_init(std::vector<int>& ori, int N) {
    int num = 0;
    bool arr[N];
    memset(arr, 0, sizeof(arr));
    for (int i = 0; i < N; i++) {
        ori[i] = 0;
    }

    for (int i = 0; i < N; i++) {
        num = rand() % N;
        while (arr[num]) {
            num = rand() % N;
        }
        arr[num] = true;
        ori[i] = num;
    }
}

int annealing_score(std::vector<int>& ori, int N) {
    std::unordered_set<int> leftdiagset;
    std::unordered_set<int> rightdigaset;
    for (int i = 0; i < N; i++) {
        leftdiagset.insert(ori[i] - i);
        rightdigaset.insert(ori[i] + i);
    }
    int score = N * 2 - (leftdiagset.size() + rightdigaset.size());
    if (score <= 1) {
        return score;
    } else  {
        return score*(score-1)/2;
    }

    return score;
}

bool annealing_local_search(int N) {
    std::vector<int> ori(N);
    for (int i = 0; i < N; i++) {
        ori[i] = 1;
    }
    const int result_vec_size = 5;
    std::vector<std::vector<int>> result_vec;
    for (int i = 0; i < result_vec_size; i++) {
        get_init(ori, N);
        result_vec.push_back(ori);
    }
    std::vector<double> result_score;
    for (int i = 0; i < result_vec.size(); i++) {
        int tmp = annealing_score(result_vec[i], N);
        result_score.push_back(tmp);
    }
    int t = 0;
    double kT = 1;
    double kTmin = 0.000000000001;
    int kCnt = N*10;
    while (kT > kTmin) {
        for (int k = 0; k < result_vec.size(); k++) {
            int preScore = result_score[k];
            if (preScore == 0) {
                return true;
            }
            int i = rand() % N;
            int j = rand() % N;
            while (j == i) {
                j = rand() % N;
            }
            std::swap(result_vec[k][i], result_vec[k][j]);
            int cur_score = annealing_score(result_vec[k], N);
            if (cur_score == 0) {
                return true;
            }
            if (preScore > cur_score) 
            {
                result_score[k] = cur_score;
                continue;
            } else {
                double result = 0;
                if (cur_score == preScore) {
                    result = 0.5;
                } else {
                    double diff = 0;
                    diff = -(cur_score - preScore);
                    diff = diff/kT;
                    result = std::exp(diff);
                }
                double prop = double(rand() % 100) / 100;
                // std::cout << diff << " " << result << " " << prop << std::endl;
                if (result > prop) {
                    result_score[k] = cur_score;
                    continue;
                }
            }
            // 还原回来找到下一个邻域
            std::swap(result_vec[k][i], result_vec[k][j]);
        }
        t++;
        kT = 0.999 * kT;
    }
    int endScore = annealing_score(ori, N);
    if (endScore == 0) {
        return true;
    }
    return false;
}

int main(int argc, const char* argv[]) {
    std::vector<int> testN = {4, 8, 16, 32, 64, 128, 256, 512};
    srand((unsigned(time(NULL))));
    clock_t start, end;
    // 退火算法
    for (int index = 0; index < testN.size(); index++) {
        int N = testN[index];
        int successCnt = 0;
        int failCnt = 0;
        int execTime = 0;
        int totalExecCnt = 10;
        for (int cnt = 0; cnt < totalExecCnt; cnt++) {
            start = clock();
            // 局部搜索算法的基础实现
            bool result = annealing_local_search(N);
            end = clock();
            if (result) {
                successCnt++;
            } else {
                failCnt++;
            }
            execTime += (end - start) / 1000;
        }
        std::cout << "局部搜索算法的退火算法 N = " << N << " 平均执行时间" << execTime / totalExecCnt << "ms" << std::endl;
        std::cout << "执行" << totalExecCnt << "次，成功次数 ：" << successCnt << " 失败次数 ：" <<  failCnt << std::endl;
    }
    return 0;
}