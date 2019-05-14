package com.asfdfdfd.defold.android.permissions;

import java.util.concurrent.atomic.AtomicInteger;
import android.Manifest;
import android.app.Activity;
import android.app.Fragment;
import android.app.FragmentManager;
import android.os.Build;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.util.Log;
import java.util.Map;
import java.util.HashMap;
import java.lang.System;

class PermissionRequestsManager {
            
    private static final AtomicInteger REQUEST_CODE = new AtomicInteger(0);

    private static PermissionRequestsManager permissionRequestsManager;

    private final Map<Integer, Long> permissionRequestContexts = new HashMap<Integer, Long>();

    synchronized static public PermissionRequestsManager getInstance() {
        if (permissionRequestsManager == null) {
            permissionRequestsManager = new PermissionRequestsManager();
        }
        return permissionRequestsManager;
    }
    
    private native void onRequestPermissionsResult(String[] permissions, int[] grantResults, long permissionRequestContextPtr);

    public synchronized void requestPermissions(final Activity activity, final String[] permissions, final long permissionRequestContextPtr) {
        if (Build.VERSION.SDK_INT < 23) {
            final int[] grantResults = new int[permissions.length];
            for (int permissionIndex = 0; permissionIndex < permissions.length; permissionIndex++) {
                grantResults[permissionIndex] = ContextCompat.checkSelfPermission(activity, permissions[permissionIndex]);
            }
            onRequestPermissionsResult(permissions, grantResults, permissionRequestContextPtr);
        } else {
            final int requestCode = REQUEST_CODE.incrementAndGet();
            permissionRequestContexts.put(requestCode, permissionRequestContextPtr);

            final FragmentManager fragmentManager = activity.getFragmentManager();
            final Fragment fragment = fragmentManager.findFragmentByTag(PermissionsFragment.TAG);
            fragment.requestPermissions(permissions, requestCode);
        }
    }
    
    public synchronized void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        final Long permissionRequestContextPtr = permissionRequestContexts.get(requestCode);
        if (permissionRequestContextPtr != null) {
            permissionRequestContexts.remove(requestCode);
            onRequestPermissionsResult(permissions, grantResults, permissionRequestContextPtr);
        }
    }
}
