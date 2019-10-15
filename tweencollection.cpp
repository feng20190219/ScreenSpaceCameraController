#include "tweencollection.h"
#include "tweenjs.h"
#include "timestamp.h"

Tween::Tween(QObject *parent) : QObject(parent)
{
}

Tween::~Tween()
{
    if(_tweenjs)
        delete _tweenjs;
}

Tween::Tween(TweenJS *tweenjs, double startObject, double stopObject, double duration, double delay,
             TweenAction1Double easingFunction, TweenAction1 update, TweenAction complete, TweenAction cancel)
{
    _tweenjs = tweenjs;

    _startObject = startObject;
    _stopObject = stopObject;

    _duration = duration;
    _delay = delay;
    _easingFunction = easingFunction;

    _update = update;
    _complete = complete;

    _cancle = cancel;
}

void Tween::cancelTween(TweenCollection *tweenCollection)
{
    tweenCollection->removeTween(this);
}

TweenCollection::TweenCollection()
{
}

void TweenCollection::update()
{
    int i = 0;
    quint64 time = getTimestamp();
    while (i < _tweens.length()) {
        Tween *tween = _tweens[i];
        TweenJS *tweenjs = tween->_tweenjs;

        if (tween->needsStart) {
            tween->needsStart = false;
            tweenjs->start(time);
        } else if (tweenjs->update(time)) {
            ++i;
        } else {
            tweenjs->stop();
            _tweens.removeAt(i);
            delete tween;
        }
    }
}

void TweenCollection::removeTween(Tween *tween)
{
    _tweens.removeOne(tween);
}

Tween *TweenCollection::add(Tween *options)
{
    if (options->_duration == 0.0) {
        if (options->_complete) {
            options->_complete();
        }
        delete options;
        return nullptr;
    }

    double duration = options->_duration / SECONDS_PER_MILLISECOND;
    double delayInSeconds = options->_delay;
    double delay = delayInSeconds / SECONDS_PER_MILLISECOND;

    TweenAction1Double easingFunction = options->_easingFunction;
    if(!easingFunction)
        easingFunction = [=](double k){
            return TweenJS::EasingLinearNone(k);
        };

    double value = options->_startObject;
    TweenJS *tweenjs = new TweenJS(value);
    tweenjs->to(options->_stopObject, duration);
    tweenjs->delay(delay);
    tweenjs->easing(easingFunction);
    if (options->_update) {
        TweenAction1 copyUpdate = options->_update;
        TweenAction1 action = [=](double k){
            copyUpdate(k);
        };
        tweenjs->onUpdate(action);
    }
    tweenjs->onComplete(options->_complete);
    tweenjs->repeat(0.0);

    Tween *tween = new Tween(tweenjs, options->_startObject, options->_stopObject, options->_duration, delayInSeconds, easingFunction, options->_update, options->_complete, options->_cancle);
    _tweens.append(tween);

    delete options;
    return tween;
}


