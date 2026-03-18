#ifndef __KALMAN_FILTER_H
#define __KALMAN_FILTER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 卡尔曼滤波器结构体（一维）
 */
typedef struct {
    float x;        // 状态估计值
    float P;        // 估计误差协方差
    float A;        // 状态转移模型
    float H;        // 观测模型
    float Q;        // 过程噪声协方差
    float R;        // 测量噪声协方差
    float K;        // 卡尔曼增益
} KalmanFilter;

/**
 * @brief 初始化卡尔曼滤波器
 * @param kf: 滤波器结构体指针
 * @param q: 过程噪声方差
 * @param r: 测量噪声方差
 * @param initial_value: 初始估计值
 */
void KalmanFilter_Init(KalmanFilter* kf, float q, float r, float initial_value);

/**
 * @brief 更新滤波器并返回新的估计值
 * @param kf: 滤波器结构体指针
 * @param z: 当前测量值（原始传感器读数）
 * @return 经过卡尔曼滤波后的最优估计值
 */
float KalmanFilter_Update(KalmanFilter* kf, float z);

#ifdef __cplusplus
}
#endif

#endif /* __KALMAN_FILTER_H */
