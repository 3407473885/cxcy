#include "kalman_filter.h"
#include <string.h>

void KalmanFilter_Init(KalmanFilter* kf, float q, float r, float initial_value)
{
    // 清零结构体
    memset(kf, 0, sizeof(KalmanFilter));

    // 设置初始状态
    kf->x = initial_value;  // 初始估计值
    kf->P = 1.0f;           // 初始估计误差协方差
    kf->A = 1.0f;           // 状态转移系数
    kf->H = 1.0f;           // 观测矩阵
    kf->Q = q;              // 过程噪声
    kf->R = r;              // 测量噪声
}

float KalmanFilter_Update(KalmanFilter* kf, float z)
{
    // 1. 预测阶段 —— 状态预测
    // x_pred = A * x （A=1 → x 不变）
    
    // 2. 预测误差协方差
    kf->P = kf->A * kf->P * kf->A + kf->Q;

    // 3. 计算卡尔曼增益
    kf->K = kf->P * kf->H / (kf->H * kf->P * kf->H + kf->R);

    // 4. 更新状态估计
    kf->x = kf->x + kf->K * (z - kf->H * kf->x);

    // 5. 更新估计误差协方差
    kf->P = (1.0f - kf->K * kf->H) * kf->P;

    // 返回最优估计值
    return kf->x;
}
