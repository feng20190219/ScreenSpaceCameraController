#include "tweenjs.h"

TweenJS::TweenJS()
{
}

TweenJS::TweenJS(double object)
{
    if(defined(object)) {
        _object = object;
        _valuesStart = object;
    }

    _easingFunction = [=](double k) {
        return TweenJS::EasingLinearNone(k);
    };
}

void TweenJS::to(double properties, double duration)
{
    if(defined(duration))
        _duration = duration;

    _valuesEnd = properties;
}

void TweenJS::start(double time)
{
    _isPlaying = true;

    _startTime =  time;
    _startTime += _delayTime;

    if(defined(_valuesEnd)) {
        _valuesStart = _object;
    }
}

void TweenJS::stop()
{
    if ( !_isPlaying ) {
        return;
    }

    _isPlaying = false;
}

void TweenJS::delay(double amount)
{
    _delayTime = amount;
}

void TweenJS::easing(TweenAction1Double easing)
{
    _easingFunction = easing;
}

void TweenJS::onUpdate(TweenAction1 callback)
{
    _onUpdateCallback = callback;
}

void TweenJS::onComplete(TweenAction callback)
{
    _onCompleteCallback = callback;
}

void TweenJS::repeat(double times)
{
    _repeat = times;
}

bool TweenJS::update(double time)
{
    if (time < _startTime) {
        return true;
    }

    double elapsed = ( time - _startTime ) / _duration;
    elapsed = elapsed > 1.0 ? 1.0 : elapsed;

    double value = _easingFunction(elapsed);

    if(defined(_valuesEnd)) {
        double start = _valuesStart;
        double end = _valuesEnd;

        _object = start + ( end - start ) * value;
    }

    if (_onUpdateCallback) {
        _onUpdateCallback(_object);
    }

    if ( elapsed == 1.0 ) {
        if (_onCompleteCallback) {
            _onCompleteCallback();
        }

        return false;
    }

    return true;
}

double TweenJS::EasingLinearNone(double k)
{
    return  k;
}

double TweenJS::EasingCubicOut(double k)
{
    --k;
    return k * k * k + 1;
}

double TweenJS::EasingQuinticInOut(double k)
{
    if ( ( k *= 2 ) < 1 )
        return 0.5 * k * k * k * k * k;
    k -= 2;
    return 0.5 * ( k * k * k * k * k + 2 );
}
