#ifndef TWEENCOLLECTION_H
#define TWEENCOLLECTION_H

#include <QtCore>
#include "screenspaceeventutils.h"

class TweenCollection;
class TweenJS;

/**
 * @brief 动画类, 使用插值函数对动画对象的两个属性(开始时间和结束时间)进行插值运算
 *
 */
class Tween : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     *
     * @param parent 父类指针
     */
    Tween(QObject *parent = nullptr);

    /**
     * @brief 析构函数
     *
     */
    ~Tween();

    /**
     * @brief 带参构造
     *
     * @param tweenjs 动画插值函数类
     * @param startObject 动画开始时间
     * @param stopObject 动画结束时间
     * @param duration 动画持续的时间
     * @param delay 动画的延迟时间
     * @param easingFunction 动画插值函数
     * @param update update回调函数
     * @param complete complete回调函数
     * @param cancel cancel回调函数
     */
    Tween(TweenJS *tweenjs,
          double startObject,
          double stopObject,
          double duration,
          double delay,
          TweenAction1Double easingFunction,
          TweenAction1 update,
          TweenAction complete,
          TweenAction cancel);

    /**
     * @brief 取消动画, 把该动画从动画集合中移除
     *
     * @param tweenCollection 动画集合
     */
    void cancelTween(TweenCollection *tweenCollection);

    TweenJS *_tweenjs = nullptr;
    double _startObject = Math::EPSILON20;
    double _stopObject = Math::EPSILON20;
    double _duration = 0.0;
    double _delay = 0.0;
    TweenAction1Double _easingFunction = nullptr;
    TweenAction1 _update = nullptr;
    TweenAction _complete = nullptr;
    TweenAction _cancle = nullptr;
    bool needsStart = true;
};

/**
 * @brief 动画的合集类
 *
 */
class TweenCollection
{
public:
    /**
     * @brief 默认构造
     *
     */
    TweenCollection();

    /**
     * @brief update函数
     *
     */
    void update();

    /**
     * @brief 从动画合集中移除指定的动画
     *
     * @param tween 需要移除的动画
     */
    void removeTween(Tween *tween);

    /**
     * @brief 添加一个动画到动画合集中
     *
     * @param options 需要添加的动画
     * @return Tween 返回这个动画的指针
     */
    Tween *add(Tween *options);

private:
    QVector<Tween *> _tweens;
    const double SECONDS_PER_MILLISECOND = 0.001;
};

#endif // TWEENCOLLECTION_H
