Defold Android Permissions
--------------------------

This is `Native Extension <https://www.defold.com/manuals/extensions/>`_ for the `Defold Game Engine <https://www.defold.com>`_ that adds support for `Android Runtime Permissions <https://developer.android.com/guide/topics/permissions/overview>`_.

.. raw:: html

    <a href="https://www.patreon.com/bePatron?u=2321396"><img src="https://c5.patreon.com/external/logo/become_a_patron_button.png" alt="Become a Patron!"/></a>

Setup
=====

You can use extension in your own project by adding this project as a `Defold library dependency <https://www.defold.com/manuals/libraries/>`_. Open your game.project file and in the dependencies field under project add https://github.com/asfdfdfd/defold-android-permissions/archive/master.zip or point to the ZIP file of a `specific release <https://github.com/asfdfdfd/defold-android-permissions/releases>`_.

Implementation details
======================

Before requesting permissions you have to add them to the AndroidManifest.xml.

If you are working on cross-platform application the best practice is to check the existence of `android_permissions` module, this module exists only in android bundle:

.. code:: lua

    if android_permissions then
      -- call android_permissions methods
    end

API
===

android_permissions.is_permission_granted(permission)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Returns 'true' if permission is granted.

.. code:: lua

     android_permissions.is_permission_granted("android.permission.READ_CONTACTS")

android_permissions.request_permissions(permissions, callback_function)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Requests permissions from user and returns result dictionary in 'callback_function'. Dictionary key is permission name, dictionary value is 'android_permissions.PERMISSION_GRANTED' if permission request is granted, 'android_permissions.PERMISSION_DENIED' if permission request is denied and 0 if permission request is cancelled.

.. code:: lua

    local permissions = {"android.permission.WRITE_EXTERNAL_STORAGE", "android.permission.READ_CONTACTS"}

    android_permissions.request_permissions(permissions, function (results)
        for permission, grant_result in pairs(results) do
            if grant_result == android_permissions.PERMISSION_GRANTED then 
                -- do something.
            elseif grant_result == android_permissions.PERMISSION_DENIED then
                -- do something.            
            end
        end
    end)

android_permissions.should_show_request_permission_rationale(permission)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Checks whether you should show UI with rationale for requesting a permission. You should do this only if you do not have the permission and the context in which the permission is requested does not clearly communicate to the user what would be the benefit from granting this permission.

Returns 'true' if application should show rationale.

.. code:: lua

     android_permissions.should_show_request_permission_rationale("android.permission.READ_CONTACTS")