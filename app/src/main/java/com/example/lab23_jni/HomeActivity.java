package com.example.lab23_jni;

import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.content.ContextCompat;

public class HomeActivity extends AppCompatActivity {

    public native int    runSecurityProbes();
    public native String nativeGreeting();
    public native long   computeFactorial(int value);

    static {
        System.loadLibrary("shieldcore");
    }

    private TextView     bannerLabel;
    private TextView     probeDetail;
    private LinearLayout safeZone;
    private TextView     greetingOutput;
    private TextView     factOutput;
    private Button       factBtn;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.screen_home);

        bannerLabel    = findViewById(R.id.banner_label);
        probeDetail    = findViewById(R.id.probe_detail);
        safeZone       = findViewById(R.id.safe_zone);
        greetingOutput = findViewById(R.id.greeting_output);
        factOutput     = findViewById(R.id.fact_output);
        factBtn        = findViewById(R.id.btn_compute);

        evaluateEnvironment();

        factBtn.setOnClickListener(v -> triggerFactorial());
    }

    private void evaluateEnvironment() {
        int vector = runSecurityProbes();

        switch (vector) {
            case 0:
                renderSafeState();
                break;
            case 1:
                renderAlertState("Supervision active detectee (traceur)");
                break;
            case 2:
                renderAlertState("Bibliotheque d'instrumentation presente");
                break;
            case 3:
                renderAlertState("Environnement compromis — signaux multiples");
                break;
            default:
                renderAlertState("Signal inconnu — execution bloquee");
        }
    }

    private void renderSafeState() {
        bannerLabel.setText("Environnement sain");
        bannerLabel.setTextColor(ContextCompat.getColor(this, R.color.shield_ok));
        probeDetail.setText("Aucune anomalie detectee par les sondes natives.");
        safeZone.setVisibility(View.VISIBLE);
        greetingOutput.setText(nativeGreeting());
        factOutput.setText("Appuyez sur le bouton pour calculer.");
    }

    private void renderAlertState(String reason) {
        bannerLabel.setText("Alerte securite");
        bannerLabel.setTextColor(ContextCompat.getColor(this, R.color.shield_alert));
        probeDetail.setText("Motif : " + reason);
        safeZone.setVisibility(View.GONE);
    }

    private void triggerFactorial() {
        long result = computeFactorial(12);
        if (result < 0) {
            factOutput.setText("Valeur invalide.");
        } else {
            factOutput.setText("12! = " + result);
        }
    }
}