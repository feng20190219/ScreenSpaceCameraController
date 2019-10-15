#ifndef SCREENSPACECAMERACONTROLLER_H
#define SCREENSPACECAMERACONTROLLER_H

#include "sscc_global.h"
#include "screenspaceeventutils.h"
#include "float.h"
#include "cartesian3.h"
#include "rectangle.h"
#include "ray.h"
#include "licameracontroller.h"

class CameraEventAggregator;
class LiScene;
class LiCamera;
class Ellipsoid;
class Globe;
class Tween;
class TweenCollection;
class CameraController;
class LiWidget;
class LiInputSystem;

/**
 * @brief 根据窗口的鼠标输入修改相机位置和方向
 *
 */
class CONTROLLER_EXPORT ScreenSpaceCameraController : public LiCameraController
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     *
     * @param parent 父类指针
     */
    explicit ScreenSpaceCameraController(LiNode *parent = nullptr);

    /**
     * @brief 析构函数
     *
     */
    ~ScreenSpaceCameraController() override;

    /**
     * @brief update函数
     *
     */
    void update() override;

    /**
     * @brief 返回成员变量 _enableInputs
     *
     * @return bool true: _enableInputs为true, false: _enableInputs为false
     */
    bool enableInputs() const override;

    /**
     * @brief 开启或禁用相机的所有鼠标操作
     *
     * @param enable true: 开启, false: 禁用
     */
    void setEnableInputs(bool enable) override;

    /**
     * @brief 返回成员变量 _enableZoom
     *
     * @return bool true: _enableZoom为true, false: _enableZoom为false
     */
    bool enableZoom() const override;

    /**
     * @brief 开启或禁用相机的缩放操作
     *
     * @param enable true: 开启, false: 禁用
     */
    void setEnableZoom(bool enable) override;

    /**
     * @brief 返回成员变量 _enableRotate
     *
     * @return bool true: _enableRotate为true, false: _enableRotate为false
     */
    bool enableRotate() const override;

    /**
     * @brief 开启或禁用相机的旋转操作
     *
     * @param enable true: 开启, false: 禁用
     */
    void setEnableRotate(bool enable) override;


    /**
     * @brief 返回成员变量 _enableTilt
     *
     * @return bool true: _enableTilt为true, false: _enableTilt为false
     */
    bool enableTilt() const override;

    /**
     * @brief 开启或禁用相机的倾斜操作
     *
     * @param enable true: 开启, false: 禁用
     */
    void setEnableTilt(bool enable) override;

    /**
     * @brief 返回成员变量 _enableLook
     *
     * @return bool true: _enableLook为true, false: _enableLook为false
     */
    bool enableLook() const override;

    /**
     * @brief 开启或禁用相机的环视操作
     *
     * @param enable true: 开启, false: 禁用
     */
    void setEnableLook(bool enable) override;

    /**
     * @brief 返回成员变量 _enableUnderGround
     *
     * @return bool true: _enableUnderGround为true, false: _enableUnderGround为false
     */
    bool enableUnderGround() const override;

    /**
     * @brief 开启或禁用相机的地下操作
     *
     * @param enable true: 开启, false: 禁用
     */
    void setEnableUnderGround(bool enable) override;

    /**
     * @brief 返回成员变量 _enablePan
     *
     * @return bool true: _enablePan为true, false: _enablePan为false
     */
    bool enablePan() const override;

    /**
     * @brief 开启或禁用相机的平移操作
     *
     * @param enable true: 开启, false: 禁用
     */
    void setEnablePan(bool enable) override;

    /**
     * @brief 返回成员变量 _minimumCollisionTerrainHeight
     *
     * @return double 返回测试与地形碰撞前相机必须达到的最小高度
     */
    double minimumCollisionTerrainHeight() const override;

    /**
     * @brief 设置测试与地形碰撞前相机必须达到的最小高度
     *
     * @param minimumCollisionTerrainHeight 测试与地形碰撞前相机必须达到的最小高度
     */
    void setMinimumCollisionTerrainHeight(double minimumCollisionTerrainHeight) override;

    /**
     * @brief 获取相机更新后的世界坐标
     *
     * @return Vector3 返回Vector3类型
     */
    Vector3 positionWC() override;

    /**
     * @brief 获取相机更新后的y轴方向
     *
     * @return Vector3 返回Vector3类型
     */
    Vector3 directionWC() override;

    /**
     * @brief 获取相机更新后的z轴方向
     *
     * @return Vector3 返回Vector3类型
     */
    Vector3 upWC() override;

    /**
     * @brief 获取相机更新后的x轴方向
     *
     * @return Vector3 返回Vector3类型
     */
    Vector3 rightWC() override;

    /**
     * @brief 获取相机更新后的Cartographic坐标
     *
     * @return Cartographic 返回Cartographic类型
     */
    Cartographic positionCartographic() override;

    /**
     * @brief 获取相机的 heading
     *
     * @return double 返回double类型
     */
    double heading() override;

    /**
     * @brief 获取相机的 pitch
     *
     * @return double 返回double类型
     */
    double pitch() override;

    /**
     * @brief 获取相机的 roll
     *
     * @return double 返回double类型
     */
    double roll() override;

    /**
     * @brief 从相机向一个屏幕点(x, y)发射一条射线 (相机拾取)
     *
     * @param x 屏幕坐标的 x
     * @param y 屏幕坐标的 y
     * @return Ray 射线
     */
    Q_INVOKABLE Ray getPickRay(double x, double y) override;

    /**
     * @brief 改变相机的状态(位置, 偏转角度等)
     *
     * @param destination 目标点
     * @param heading 相机的heading
     * @param pitch 相机的pitch
     * @param roll 相机的roll
     */
    Q_INVOKABLE void setView(const Cartesian3 &destination, double heading, double pitch, double roll) override;

    /**
     * @brief 相机围绕axis轴旋转
     *
     * @param axis 围绕轴
     * @param angle 旋转角度 (弧度)
     */
    Q_INVOKABLE void rotate(const Vector3 &axis, double angle) override;

    /**
     * @brief 相机围绕自身中心点向上旋转
     *
     * @param angle 旋转角度 (弧度)
     */
    Q_INVOKABLE void rotateUp(double angle) override;

    /**
     * @brief 相机围绕自身中心点向下旋转
     *
     * @param angle 旋转角度 (弧度)
     */
    Q_INVOKABLE void rotateDown(double angle) override;

    /**
     * @brief 相机围绕自身中心点向右旋转
     *
     * @param angle 旋转角度 (弧度)
     */
    Q_INVOKABLE void rotateRight(double angle) override;

    /**
     * @brief 相机围绕自身中心点向左旋转
     *
     * @param angle 旋转角度 (弧度)
     */
    Q_INVOKABLE void rotateLeft(double angle) override;

    /**
     * @brief 相机沿着dir方向平移
     *
     * @param dir 平移的方向
     * @param amount 平移量
     */
    Q_INVOKABLE void move(const Vector3 &dir, double amount) override;

    /**
     * @brief 相机沿着y轴方向平移
     *
     * @param amount 平移量
     */
    Q_INVOKABLE void moveForward(double amount) override;

    /**
     * @brief 相机沿着y轴反方向平移
     *
     * @param amount 平移量
     */
    Q_INVOKABLE void moveBackward(double amount) override;

    /**
     * @brief 相机沿着z轴方向平移
     *
     * @param amount 平移量
     */
    Q_INVOKABLE void moveUp(double amount) override;

    /**
     * @brief 相机沿着z轴反方向平移
     *
     * @param amount 平移量
     */
    Q_INVOKABLE void moveDown(double amount) override;

    /**
     * @brief 相机沿着x轴方向平移
     *
     * @param amount 平移量
     */
    Q_INVOKABLE void moveRight(double amount) override;

    /**
     * @brief 相机沿着x轴反方向平移
     *
     * @param amount 平移量
     */
    Q_INVOKABLE void moveLeft(double amount) override;

    /**
     * @brief 相机的每一个方向(x、y、z)分别围绕axis轴旋转
     *
     * @param axis 围绕轴
     * @param angle 旋转角度 (弧度)
     */
    Q_INVOKABLE void look(const Vector3 &axis, double angle) override;

    /**
     * @brief 相机围绕x轴旋转
     *
     * @param amount
     */
    Q_INVOKABLE void lookUp(double amount) override;

    /**
     * @brief 相机围绕x轴反方向旋转
     *
     * @param amount 旋转量 (弧度)
     */
    Q_INVOKABLE void lookDown(double amount) override;

    /**
     * @brief 相机围绕z轴旋转
     *
     * @param amount 旋转量 (弧度)
     */
    Q_INVOKABLE void lookRight(double amount) override;

    /**
     * @brief 相机围绕z轴反方向旋转
     *
     * @param amount 旋转量 (弧度)
     */
    Q_INVOKABLE void lookLeft(double amount) override;

    /**
     * @brief 相机沿着y轴方向平移
     *
     * @param amount 平移量
     */
    Q_INVOKABLE void zoomIn(double amount) override;

    /**
     * @brief 相机沿着y轴反方向平移
     *
     * @param amount 平移量
     */
    Q_INVOKABLE void zoomOut(double amount) override;

    /**
     * @brief 相机飞到目标点 (Vector3类型)
     *
     * @param destination 目标点
     * @param duration 飞行时间 (秒)
     * @param heading 飞到目标点后, 更新相机的heading
     * @param pitch 飞到目标点后, 更新相机的pitch
     * @param roll 飞到目标点后, 更新相机的roll
     */
    Q_INVOKABLE void flyTo(const Vector3 &destination, double duration  = Math::EPSILON20, double heading = 0, double pitch = -90, double roll = 0) override;

    /**
     * @brief 相机飞到目标点 (Cartographic类型)
     *
     * @param destination 目标点
     * @param duration 飞行时间 (秒)
     * @param heading 飞到目标点后, 更新相机的heading
     * @param pitch 飞到目标点后, 更新相机的pitch
     * @param roll 飞到目标点后, 更新相机的roll
     */
    Q_INVOKABLE void flyTo(const Cartographic &destination, double duration  = Math::EPSILON20, double heading = 0, double pitch = -90, double roll = 0) override;

    /**
     * @brief 相机飞到目标点 (LiRectangle类型)
     *
     * @param destination 目标点
     * @param duration 飞行时间 (秒)
     * @param heading 飞到目标点后, 更新相机的heading
     * @param pitch 飞到目标点后, 更新相机的pitch
     * @param roll 飞到目标点后, 更新相机的roll
     */
    Q_INVOKABLE void flyTo(const LiRectangle &destination, double duration  = Math::EPSILON20, double heading = 0, double pitch = -90, double roll = 0) override;

    /**
     * @brief 拾取, 同样适用于地下模式的拾取
     *
     * @param mousePosition 屏幕坐标点
     * @return Cartesian3 世界坐标点
     */
    Q_INVOKABLE Cartesian3 pickGlobe(const Vector2 &mousePosition) const override;

    bool _enableInputs = true; ///< 开启或禁用相机的所有鼠标操作, true: 开启, false: 禁用
    double _minimumCollisionTerrainHeight = 15000.0; ///< 测试与地形碰撞前相机必须达到的最小高度

public slots:
    void spin3DByKey(double startX, double startY, double endX, double endY, bool touring = false, bool mouseUp = false);

private:
    bool m_touring = false;
    bool m_looking = false;
    void pan3DByKey(double startX, double startY, double endX, double endY, Ellipsoid *ellipsoid);
    void rotate3DByKey(double startX, double startY, double endX, double endY);
    void look3DByKey(double startX, double startY, double endX, double endY);

    void update3D();

    struct EventType {
        EventType() {
            eventType = CameraEventType::TypeCount;
            modifier = 0;
        }

        EventType(CameraEventType::Type t, int m) {
            eventType = t;
            modifier = m;
        }

        CameraEventType::Type eventType;
        int modifier;
    };

    typedef std::function<void(const Cartesian2 &, const CameraMovement &)> Action;

    void reactToInput(bool enabled,
                      const QVector<EventType> &eventTypes,
                      Action action,
                      double inertiaConstant = 1.0,
                      const QString &inertiaStateName = QString());

    void maintainInertia(CameraEventType::Type type,
                         int modifier,
                         double decayCoef,
                         Action action,
                         const QString &lastMovementName);

    double decay(double time, double coefficient) const;

    void spin3D(const Cartesian2 &startPosition, const CameraMovement &movement);
    void zoom3D(const Cartesian2 &startPosition, const CameraMovement &movement);
    void tilt3D(const Cartesian2 &startPosition, const CameraMovement &movement);
    void look3D(const Cartesian2 &startPosition, const CameraMovement &movement);
    void strafe(const CameraMovement &movement);
    void pan3D(const CameraMovement &movement, Ellipsoid *ellipsoid = nullptr);
    void rotate3D(const CameraMovement &movement,
                  const Cartesian3 &constrainedAxis = Cartesian3(Math::EPSILON20, 0, 0),
                  bool rotateOnlyVertical = false,
                  bool rotateOnlyHorizontal = false);

    void tilt3DOnEllipsoid(const Cartesian2 &startPosition, const CameraMovement &movement);
    void tilt3DOnTerrain(const Cartesian2 &startPosition, const CameraMovement &movement);
    void handleZoom(const Cartesian2 &startPosition,
                    const CameraMovement &movement,
                    double zoomFactor,
                    double distanceMeasure,
                    double unitPositionDotDirection);

    void handleKeyDown();

    struct MovementState {
        Cartesian2 startPosition;
        Cartesian2 endPosition;
        Cartesian2 motion;
        bool active = false;
    };

    QVector<EventType> translateEventTypes;
    QVector<EventType> zoomEventTypes;
    QVector<EventType> rotateEventTypes;
    QVector<EventType> tiltEventTypes;
    QVector<EventType> lookEventTypes;

    TweenCollection *_tweens;
    CameraEventAggregator *_aggregator;
    LiInputSystem *_input;
    LiScene *_scene;
    LiWidget *_canvas;

    Globe *_globe;
    Globe *m_globe;
    LiCamera *_camera;
    LiTransform *_cameraTrans;
    Ellipsoid *_ellipsoid;
    Ellipsoid *_sphereEllipsoid;

    CameraController *_cameraController;
    QHash<QString, MovementState*> _movementState;

    Cartesian3 _rotationAxis;

//    bool enableTranslate = true;
    bool _enableZoom = true;
    bool _enableRotate = true;
    bool _enableTilt = true;
    bool _enableLook = true;
    double inertiaSpin = 0.9;
//    double inertiaTranslate = 0.9;
    double inertiaZoom = 0.8;
    double maximumMovementRatio = 0.1;
    double minimumZoomDistance = 1.0;
    double maximumZoomDistance = DBL_MAX;

    double _minimumPickingTerrainHeight = 150000.0;
    double _minimumTrackBallHeight = 7500000.0;

    Cartesian2 _tiltCenterMousePosition = Cartesian2(-1.0, -1.0);
    Cartesian3 _tiltCenter;
    Cartesian2 _rotateMousePosition = Cartesian2(-1.0, -1.0);
    Cartesian3 _rotateStartPosition;
    Cartesian3 _strafeStartPosition;
    Cartesian2 _zoomMouseStart = Cartesian2(-1.0, -1.0);
    Cartesian3 _zoomWorldPosition;
    bool _useZoomWorldPosition = false;
    bool _looking = false;
    bool _rotating = false;
    bool _strafing = false;
    bool _zoomingOnVector = false;
    bool _rotatingZoom = false;
    bool _tiltOnEllipsoid = false;

    double _zoomFactor = 5.0;
    double _rotateFactor = Math::EPSILON20;
    double _rotateRateRangeAdjustment = Math::EPSILON20;
    double _maximumRotateRate = 1.77;
    double _minimumRotateRate = 0.0002;
    double _minimumZoomRate = 20.0;
    double _maximumZoomRate = 5906376272000.0;  // distance from the Sun to Pluto in meters.

    // add underground
    bool _enableUnderGround = false;
    bool _enablePan = true;

    quint64 lastTime = 0;
//    double earthRadius = 6378137.0;
    double maxCameraHeight = 62000000.0;
//    double minCameraHeight = 1.5;
    double k1 = 8.0;
//    double k2 = 70.0;
//    double fovy = Math::toRadians(42.1034);
};

extern "C" {
/**
 * @brief 全局函数, 把ScreenSpaceCameraConroller对象添加到LiCamera对象中
 *
 */
void CONTROLLER_EXPORT loadPlugin();
//void CONTROLLER_EXPORT spin3DByKeyFunc(void *s, double startX, double startY, double endX, double endY, bool mouseUp);
}

#endif // SCREENSPACECAMERACONTROLLER_H
