#ifndef TWEENJS_H
#define TWEENJS_H

#include "screenspaceeventutils.h"

/**
 * @brief 动画插值函数类
 *
 */
class TweenJS
{
public:
    /**
     * @brief  默认构造
     *
     */
    TweenJS();

    /**
     * @brief  带参构造
     *
     * @param object 参数值,double类型
     */
    TweenJS(double object);

    /**
     * @brief 设置动画的结束时间和持续时间
     *
     * @param properties 动画的结束时间
     * @param duration 动画的持续时间
     */
    void to(double properties, double duration);

    /**
     * @brief 设置动画的开始时间
     *
     * @param time 动画的开始时间
     */
    void start(double time);

    /**
     * @brief 动画结束
     *
     */
    void stop();

    /**
     * @brief 设置动画的延迟时间
     *
     * @param amount 动画的延迟时间
     */
    void delay(double amount);

    /**
     * @brief 设置动画采用的插值函数
     *
     * @param easing 插值函数
     */
    void easing(TweenAction1Double easing);

    /**
     * @brief update回调函数
     *
     * @param callback 回调执行的函数
     */
    void onUpdate(TweenAction1 callback);

    /**
     * @brief complete回调函数
     *
     * @param callback 回调执行的函数
     */
    void onComplete(TweenAction callback);

    /**
     * @brief 设置动画重复次数
     *
     * @param times 重复次数
     */
    void repeat(double times);

    /**
     * @brief update函数
     *
     * @param time 时间参数
     * @return bool true: 动画还没结束, 下一帧继续执行update函数, false: 动画结束, update函数不再执行
     */
    bool update(double time);

    /**
     * @brief EasingLinearNone插值函数
     *
     * @param k 插值参数, double类型
     * @return double 返回插值的结果, double类型
     */
    static double EasingLinearNone(double k);

    /**
     * @brief EasingCubicOut插值函数
     *
     * @param k 插值参数, double类型
     * @return double 返回插值的结果, double类型
     */
    static double EasingCubicOut(double k);

    /**
     * @brief EasingQuinticInOut插值函数
     *
     * @param k 插值参数, double类型
     * @return double 返回插值的结果, double类型
     */
    static double EasingQuinticInOut(double k);

private:
    double _object = Math::EPSILON20;
    double _valuesStart = Math::EPSILON20;
    double _valuesEnd = Math::EPSILON20;
    double _duration = 1000;
    double _repeat = 0.0;
    bool _isPlaying = false;
    double _delayTime = 0.0;
    double _startTime = 0.0;
    TweenAction1Double _easingFunction = nullptr;
    TweenAction1 _onUpdateCallback = nullptr;
    TweenAction _onCompleteCallback = nullptr;
};

#endif // TWEENJS_H
