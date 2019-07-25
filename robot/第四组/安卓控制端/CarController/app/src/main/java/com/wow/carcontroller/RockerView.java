package com.wow.carcontroller;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.PixelFormat;
import android.graphics.Point;
import android.graphics.Rect;
import android.graphics.drawable.Drawable;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;

/**
 * reference https://github.com/kongqw/AndroidRocker
 */

public class RockerView extends View {

    private static final int DEFAULT_SIZE = 400;
    private static final int DEFAULT_ROCKER_RADIUS = DEFAULT_SIZE / 8;

    private Paint mAreaBackgroundPaint;
    private Paint mRockerPaint;

    private Point mRockerPosition;
    private Point mCenterPoint;

    private int mAreaRadius;
    private int mRockerRadius;

    private OnShakeListener mOnShakeListener;

    private Direction tempDirection = Direction.DIRECTION_CENTER;

    // 角度
    private static final double ANGLE_0 = 0;
    private static final double ANGLE_360 = 360;

    // 360°平分4份的边缘角度(旋转45度)
    private static final double ANGLE_ROTATE45_4D_OF_0P = 45;
    private static final double ANGLE_ROTATE45_4D_OF_1P = 135;
    private static final double ANGLE_ROTATE45_4D_OF_2P = 225;
    private static final double ANGLE_ROTATE45_4D_OF_3P = 315;

    // 摇杆可移动区域背景
    private static final int AREA_BACKGROUND_MODE_PIC = 0;
    private static final int AREA_BACKGROUND_MODE_XML = 2;
    private static final int AREA_BACKGROUND_MODE_DEFAULT = 3;
    private int mAreaBackgroundMode = AREA_BACKGROUND_MODE_DEFAULT;
    private Bitmap mAreaBitmap;
    // 摇杆背景
    private static final int ROCKER_BACKGROUND_MODE_PIC = 4;
    private static final int ROCKER_BACKGROUND_MODE_XML = 6;
    private static final int ROCKER_BACKGROUND_MODE_DEFAULT = 7;
    private int mRockerBackgroundMode = ROCKER_BACKGROUND_MODE_DEFAULT;
    private Bitmap mRockerBitmap;

    public RockerView(Context context, AttributeSet attrs) {
        super(context, attrs);

        // 获取自定义属性
        initAttribute(context, attrs);

        if (isInEditMode()) {
        }

        // 移动区域画笔
        mAreaBackgroundPaint = new Paint();
        mAreaBackgroundPaint.setAntiAlias(true);

        // 摇杆画笔
        mRockerPaint = new Paint();
        mRockerPaint.setAntiAlias(true);

        // 中心点
        mCenterPoint = new Point();
        // 摇杆位置
        mRockerPosition = new Point();
    }

    /**
     * 获取属性
     *
     * @param context context
     * @param attrs   attrs
     */
    private void initAttribute(Context context, AttributeSet attrs) {
        TypedArray typedArray = context.obtainStyledAttributes(attrs, R.styleable.RockerView);

        // 可移动区域背景
        Drawable areaBackground = typedArray.getDrawable(R.styleable.RockerView_areaBackground);
        mAreaBitmap = drawable2Bitmap(areaBackground);
        mAreaBackgroundMode = AREA_BACKGROUND_MODE_XML;
        // 摇杆背景
        Drawable rockerBackground = typedArray.getDrawable(R.styleable.RockerView_rockerBackground);
        mRockerBitmap = drawable2Bitmap(rockerBackground);
        mRockerBackgroundMode = ROCKER_BACKGROUND_MODE_XML;

        // 摇杆半径
        mRockerRadius = typedArray.getDimensionPixelOffset(R.styleable.RockerView_rockerRadius, DEFAULT_ROCKER_RADIUS);

        typedArray.recycle();
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        int measureWidth, measureHeight;

        int widthMode = MeasureSpec.getMode(widthMeasureSpec);
        int heightMode = MeasureSpec.getMode(heightMeasureSpec);
        int widthSize = MeasureSpec.getSize(widthMeasureSpec);
        int heightSize = MeasureSpec.getSize(heightMeasureSpec);

        if (widthMode == MeasureSpec.EXACTLY) {
            // 具体的值和match_parent
            measureWidth = widthSize;
        } else {
            // wrap_content
            measureWidth = DEFAULT_SIZE;
        }
        if (heightMode == MeasureSpec.EXACTLY) {
            measureHeight = heightSize;
        } else {
            measureHeight = DEFAULT_SIZE;
        }
        setMeasuredDimension(measureWidth, measureHeight);
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);

        int measuredWidth = getMeasuredWidth();
        int measuredHeight = getMeasuredHeight();

        int cx = measuredWidth / 2;
        int cy = measuredHeight / 2;
        // 中心点
        mCenterPoint.set(cx, cy);
        // 可移动区域的半径
        mAreaRadius = (measuredWidth <= measuredHeight) ? cx : cy;

        // 摇杆位置
        if (0 == mRockerPosition.x || 0 == mRockerPosition.y) {
            mRockerPosition.set(mCenterPoint.x, mCenterPoint.y);
        }

        // 画可移动区域
        if (AREA_BACKGROUND_MODE_PIC == mAreaBackgroundMode || AREA_BACKGROUND_MODE_XML == mAreaBackgroundMode) {
            // 图片
            Rect src = new Rect(0, 0, mAreaBitmap.getWidth(), mAreaBitmap.getHeight());
            Rect dst = new Rect(mCenterPoint.x - mAreaRadius, mCenterPoint.y - mAreaRadius, mCenterPoint.x + mAreaRadius, mCenterPoint.y + mAreaRadius);
            canvas.drawBitmap(mAreaBitmap, src, dst, mAreaBackgroundPaint);
        } else {
            // 其他或者未设置
            mAreaBackgroundPaint.setColor(Color.GRAY);
            canvas.drawCircle(mCenterPoint.x, mCenterPoint.y, mAreaRadius, mAreaBackgroundPaint);
        }

        // 画摇杆
        if (ROCKER_BACKGROUND_MODE_PIC == mRockerBackgroundMode || ROCKER_BACKGROUND_MODE_XML == mRockerBackgroundMode) {
            // 图片
            Rect src = new Rect(0, 0, mRockerBitmap.getWidth(), mRockerBitmap.getHeight());
            Rect dst = new Rect(mRockerPosition.x - mRockerRadius, mRockerPosition.y - mRockerRadius, mRockerPosition.x + mRockerRadius, mRockerPosition.y + mRockerRadius);
            canvas.drawBitmap(mRockerBitmap, src, dst, mRockerPaint);
        } else {
            // 其他或者未设置
            mRockerPaint.setColor(Color.RED);
            canvas.drawCircle(mRockerPosition.x, mRockerPosition.y, mRockerRadius, mRockerPaint);
        }
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        switch (event.getAction()) {
            case MotionEvent.ACTION_DOWN:// 按下
                // 回调 开始
                callBackStart();
            case MotionEvent.ACTION_MOVE:// 移动
                float moveX = event.getX();
                float moveY = event.getY();
                mRockerPosition = getRockerPositionPoint(mCenterPoint, new Point((int) moveX, (int) moveY), mAreaRadius, mRockerRadius);
                moveRocker(mRockerPosition.x, mRockerPosition.y);
                break;
            case MotionEvent.ACTION_UP:// 抬起
            case MotionEvent.ACTION_CANCEL:// 移出区域
                // 回调 结束
                callBackFinish();
                moveRocker(mCenterPoint.x, mCenterPoint.y);
                break;
        }
        return true;
    }

    /**
     * 获取摇杆实际要显示的位置（点）
     *
     * @param centerPoint  中心点
     * @param touchPoint   触摸点
     * @param regionRadius 摇杆可活动区域半径
     * @param rockerRadius 摇杆半径
     * @return 摇杆实际显示的位置（点）
     */
    private Point getRockerPositionPoint(Point centerPoint, Point touchPoint, float regionRadius, float rockerRadius) {
        // 两点在X轴的距离
        float lenX = (float) (touchPoint.x - centerPoint.x);
        // 两点在Y轴距离
        float lenY = (float) (touchPoint.y - centerPoint.y);
        // 两点距离
        float lenXY = (float) Math.sqrt((double) (lenX * lenX + lenY * lenY));
        // 计算弧度
        double radian = Math.acos(lenX / lenXY) * (touchPoint.y < centerPoint.y ? -1 : 1);
        // 计算角度
        double angle = radian2Angle(radian);

        // 回调 返回参数
        callBack(angle);

        if (lenXY + rockerRadius <= regionRadius) { // 触摸位置在可活动范围内
            return touchPoint;
        } else { // 触摸位置在可活动范围以外
            // 计算要显示的位置
            int showPointX = (int) (centerPoint.x + (regionRadius - rockerRadius) * Math.cos(radian));
            int showPointY = (int) (centerPoint.y + (regionRadius - rockerRadius) * Math.sin(radian));
            return new Point(showPointX, showPointY);
        }
    }

    /**
     * 移动摇杆到指定位置
     *
     * @param x x坐标
     * @param y y坐标
     */
    private void moveRocker(float x, float y) {
        mRockerPosition.set((int) x, (int) y);
        invalidate();
    }

    /**
     * 弧度转角度
     *
     * @param radian 弧度
     * @return 角度[0, 360)
     */
    private double radian2Angle(double radian) {
        double tmp = Math.round(radian / Math.PI * 180);
        return tmp >= 0 ? tmp : 360 + tmp;
    }

    /**
     * Drawable 转 Bitmap
     *
     * @param drawable Drawable
     * @return Bitmap
     */
    private Bitmap drawable2Bitmap(Drawable drawable) {
        // 取 drawable 的长宽
        int width = drawable.getIntrinsicWidth();
        int height = drawable.getIntrinsicHeight();
        // 取 drawable 的颜色格式
        Bitmap.Config config = drawable.getOpacity() != PixelFormat.OPAQUE ? Bitmap.Config.ARGB_8888 : Bitmap.Config.RGB_565;
        // 建立对应 bitmap
        Bitmap bitmap = Bitmap.createBitmap(width, height, config);
        Canvas canvas = new Canvas(bitmap);
        drawable.setBounds(0, 0, width, height);
        drawable.draw(canvas);
        return bitmap;
    }

    /**
     * 回调
     * 开始
     */
    private void callBackStart() {
        tempDirection = Direction.DIRECTION_CENTER;
        if (null != mOnShakeListener) {
            mOnShakeListener.onStart();
        }
    }

    /**
     * 回调
     * 返回参数
     *
     * @param angle 摇动角度
     */
    private void callBack(double angle) {
        if (null != mOnShakeListener) {
            if ((ANGLE_0 <= angle && ANGLE_ROTATE45_4D_OF_0P > angle || ANGLE_ROTATE45_4D_OF_3P <= angle && ANGLE_360 > angle) && tempDirection != Direction.DIRECTION_RIGHT) {
                // 右
                tempDirection = Direction.DIRECTION_RIGHT;
                mOnShakeListener.direction(Direction.DIRECTION_RIGHT);
            } else if (ANGLE_ROTATE45_4D_OF_0P <= angle && ANGLE_ROTATE45_4D_OF_1P > angle && tempDirection != Direction.DIRECTION_DOWN) {
                // 下
                tempDirection = Direction.DIRECTION_DOWN;
                mOnShakeListener.direction(Direction.DIRECTION_DOWN);
            } else if (ANGLE_ROTATE45_4D_OF_1P <= angle && ANGLE_ROTATE45_4D_OF_2P > angle && tempDirection != Direction.DIRECTION_LEFT) {
                // 左
                tempDirection = Direction.DIRECTION_LEFT;
                mOnShakeListener.direction(Direction.DIRECTION_LEFT);
            } else if (ANGLE_ROTATE45_4D_OF_2P <= angle && ANGLE_ROTATE45_4D_OF_3P > angle && tempDirection != Direction.DIRECTION_UP) {
                // 上
                tempDirection = Direction.DIRECTION_UP;
                mOnShakeListener.direction(Direction.DIRECTION_UP);
            }
        }
    }

    /**
     * 回调
     * 结束
     */
    private void callBackFinish() {
        tempDirection = Direction.DIRECTION_CENTER;
        if (null != mOnShakeListener) {
            mOnShakeListener.onFinish();
        }
    }

    /**
     * 方向
     */
    public enum Direction {
        DIRECTION_LEFT, // 左
        DIRECTION_RIGHT, // 右
        DIRECTION_UP, // 上
        DIRECTION_DOWN, // 下
        DIRECTION_CENTER // 中间
    }

    /**
     * 添加摇动的监听
     *
     * @param listener      回调
     */
    public void setOnShakeListener(OnShakeListener listener) {
        mOnShakeListener = listener;
    }

    /**
     * 摇动方向监听接口
     */
    public interface OnShakeListener {
        // 开始
        void onStart();

        /**
         * 摇动方向
         *
         * @param direction 方向
         */
        void direction(Direction direction);

        // 结束
        void onFinish();
    }

}
