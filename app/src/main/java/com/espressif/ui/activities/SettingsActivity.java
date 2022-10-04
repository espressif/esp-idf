package com.espressif.ui.activities;

import android.content.SharedPreferences;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.view.MenuItem;
import android.widget.TextView;

import androidx.appcompat.app.ActionBar;
import androidx.appcompat.app.AppCompatActivity;
import androidx.preference.EditTextPreference;
import androidx.preference.Preference;
import androidx.preference.PreferenceFragmentCompat;
import androidx.preference.PreferenceManager;
import androidx.preference.SwitchPreferenceCompat;

import com.espressif.AppConstants;
import com.espressif.wifi_provisioning.R;

public class SettingsActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_settings);
        getSupportFragmentManager()
                .beginTransaction()
                .replace(R.id.settings, new SettingsFragment())
                .commit();
        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
        }

        TextView tvAppVersion = findViewById(R.id.tv_app_version);

        String version = "";
        try {
            PackageInfo pInfo = getPackageManager().getPackageInfo(getPackageName(), 0);
            version = pInfo.versionName;
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
        }

        String appVersion = getString(R.string.app_version) + " - v" + version;
        tvAppVersion.setText(appVersion);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {

        // Note : Overriding this method to make ActionBar "Back" button working.
        int id = item.getItemId();
        if (id == android.R.id.home) {
            finish();
        }
        return super.onOptionsItemSelected(item);
    }

    public static class SettingsFragment extends PreferenceFragmentCompat {

        SwitchPreferenceCompat securityPref;
        EditTextPreference userNamePref;

        @Override
        public void onCreatePreferences(Bundle savedInstanceState, String rootKey) {

            PreferenceManager prefMgr = getPreferenceManager();
            prefMgr.setSharedPreferencesName(AppConstants.ESP_PREFERENCES);
            setPreferencesFromResource(R.xml.preferences, rootKey);

            SharedPreferences sharedPreferences = prefMgr.getSharedPreferences();
            securityPref = prefMgr.findPreference(AppConstants.KEY_SECURITY_TYPE);
            userNamePref = prefMgr.findPreference(AppConstants.KEY_USER_NAME);

            boolean isSecure = sharedPreferences.getBoolean(AppConstants.KEY_SECURITY_TYPE, true);
            if (isSecure) {
                securityPref.setSummary(R.string.summary_secured);
                userNamePref.setVisible(true);
            } else {
                securityPref.setSummary(R.string.summary_unsecured);
                userNamePref.setVisible(false);
            }

            securityPref.setOnPreferenceChangeListener(new Preference.OnPreferenceChangeListener() {

                @Override
                public boolean onPreferenceChange(Preference preference, Object newValue) {
                    boolean isSecure = (boolean) newValue;

                    if (isSecure) {
                        preference.setSummary(R.string.summary_secured);
                        userNamePref.setVisible(true);
                    } else {
                        preference.setSummary(R.string.summary_unsecured);
                        userNamePref.setVisible(false);
                    }
                    return true;
                }
            });
        }
    }
}