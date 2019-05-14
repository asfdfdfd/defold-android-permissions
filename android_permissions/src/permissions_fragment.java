package com.asfdfdfd.defold.android.permissions;

import android.app.Activity;
import android.app.Fragment;
import android.app.FragmentManager;
import android.app.FragmentTransaction;
import android.view.View;
import android.view.ViewGroup;
import android.view.LayoutInflater;
import android.widget.FrameLayout;
import android.os.Bundle;
import java.lang.System;

class PermissionsFragment extends Fragment {
    
    public static final String TAG = PermissionsFragment.class.getSimpleName();

    public static synchronized void initialize(final Activity activity) {
        final FragmentManager fragmentManager = activity.getFragmentManager();
        if (fragmentManager.findFragmentByTag(TAG) == null) {
            fragmentManager.beginTransaction().add(new PermissionsFragment(), TAG).commit();
        }
    }
     
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setRetainInstance(true);
    }
   
    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        PermissionRequestsManager.getInstance().onRequestPermissionsResult(requestCode, permissions, grantResults);
    }
}
