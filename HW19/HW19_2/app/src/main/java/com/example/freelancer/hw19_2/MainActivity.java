package com.example.freelancer.hw19_2;

import android.Manifest;
import android.app.Activity;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.os.Bundle;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.TextureView;
import android.view.WindowManager;
import android.widget.TextView;
import android.widget.SeekBar;

import java.io.IOException;

import static android.graphics.Color.blue;
import static android.graphics.Color.green;
import static android.graphics.Color.red;
import static android.graphics.Color.rgb;
import static android.icu.lang.UCharacter.GraphemeClusterBreak.T;

public class MainActivity extends Activity implements TextureView.SurfaceTextureListener {
    private Camera mCamera;
    private TextureView mTextureView;
    private SurfaceView mSurfaceView;
    private SurfaceHolder mSurfaceHolder;
    private Bitmap bmp = Bitmap.createBitmap(800, 800, Bitmap.Config.ARGB_8888);
    private Canvas canvas = new Canvas(bmp);
    private Paint paint1 = new Paint();
    private TextView mTextView;
    private SeekBar myControl;
    private SeekBar myT;
    private int threshold = 0;

    static long prevtime = 0; // for FPS calculation

    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON); // keeps the screen from turning off

        myControl = (SeekBar)findViewById(R.id.seek1);
        myT = (SeekBar)findViewById(R.id.seek2);
        mTextView = (TextView) findViewById(R.id.cameraStatus);
        setMyControlListener();
        // see if the app has permission to use the camera
        ActivityCompat.requestPermissions(MainActivity.this, new String[]{Manifest.permission.CAMERA}, 1);
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.CAMERA) == PackageManager.PERMISSION_GRANTED) {
            mSurfaceView = (SurfaceView) findViewById(R.id.surfaceview);
            mSurfaceHolder = mSurfaceView.getHolder();

            mTextureView = (TextureView) findViewById(R.id.textureview);
            mTextureView.setSurfaceTextureListener(this);

            // set the paintbrush for writing text on the image
            paint1.setColor(0xffff0000); // red
            paint1.setTextSize(24);

            mTextView.setText("started camera");
        } else {
            mTextView.setText("no camera permissions");
        }

    }

    public void onSurfaceTextureAvailable(SurfaceTexture surface, int width, int height) {
        mCamera = Camera.open();
        Camera.Parameters parameters = mCamera.getParameters();
        parameters.setPreviewSize(640, 480);
        parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_INFINITY); // no autofocusing
        parameters.setAutoExposureLock(false); // keep the white balance constant
        mCamera.setParameters(parameters);
        mCamera.setDisplayOrientation(90); // rotate to portrait mode

        try {
            mCamera.setPreviewTexture(surface);
            mCamera.startPreview();
        } catch (IOException ioe) {
            // Something bad happened
        }
    }

    public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width, int height) {
        // Ignored, Camera does all the work for us
    }

    public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
        mCamera.stopPreview();
        mCamera.release();
        return true;
    }

    // the important function
    public void onSurfaceTextureUpdated(SurfaceTexture surface) {
        // every time there is a new Camera preview frame
        mTextureView.getBitmap(bmp);

        final Canvas c = mSurfaceHolder.lockCanvas();
        if (c != null) {
//            int thresh = 0; // comparison value
            int[] pixels = new int[bmp.getWidth()]; // pixels[] is the RGBA data
            int counter = 0;
            int sum_mr ; // the sum of the mass times the radius
            int sum_m; // the sum of the masses
            int COM = 0;

            // in the row, see if there is more green than red
            for(int j = 0; j<bmp.getHeight();j+=3) {
                if (counter == 5) {
                    counter = 0;
                    sum_m = 0;
                    sum_mr = 0;
                    bmp.getPixels(pixels, 0, bmp.getWidth(), 0, j, bmp.getWidth(), 1);
                    for (int i = 0; i < bmp.getWidth(); i++) {
                        if (((green(pixels[i]) - red(pixels[i])) > -threshold) && ((green(pixels[i]) - red(pixels[i])) < threshold) &&
                                ((blue(pixels[i]) - red(pixels[i])) > -threshold) && ((blue(pixels[i]) - red(pixels[i])) < threshold) &&
                                ((green(pixels[i]) - blue(pixels[i])) > -threshold)&&((green(pixels[i]) - blue(pixels[i])) < threshold) && (green(pixels[i])  > T)) {
                            pixels[i] = rgb(1, 1, 1); // set the pixel to almost 100% black
                            sum_m += green(pixels[i])+red(pixels[i])+blue(pixels[i]);
                            sum_mr += (green(pixels[i])+red(pixels[i])+blue(pixels[i]))*i;
                        }
                        if(sum_m>5){
                            COM = sum_mr / sum_m;
                        }
                        else{
                            COM = 0;
                        }
                    }
                    bmp.setPixels(pixels, 0, bmp.getWidth(), 0, j, bmp.getWidth(), 1);
                    if(COM !=0) {
                        canvas.drawCircle(COM, j, 5, paint1); // x position, y position, diameter, color
                    }
                }
                counter++;
            }

            // update the row

        }

        // write the pos as text
        canvas.drawText("Threshold = " + threshold, 10, 200, paint1);
        c.drawBitmap(bmp, 0, 0, null);
        mSurfaceHolder.unlockCanvasAndPost(c);

        // calculate the FPS to see how fast the code is running
        long nowtime = System.currentTimeMillis();
        long diff = nowtime - prevtime;
        mTextView.setText("FPS " + 1000 / diff);
        prevtime = nowtime;
    }

    private void setMyControlListener(){
        myControl.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                threshold = progress;
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });
    }
}
