Mail & Discord (?) & Direct File Upload 


## How It Works
## Global Keyboard Hook:

The program sets a global keyboard hook using SetWindowsHookEx with the WH_KEYBOARD_LL type.
This allows the program to capture all keystrokes system-wide.
## Keystroke Logging:

Each keystroke is captured in the keyboardHook function.
The virtual key code (vkCode) is mapped to a character or special key name using the getKeyName function.
Keystrokes are accumulated in a string (keystrokes).
## Sending Logs via Email:

The sendEmail function sends the accumulated keystrokes to a specified email address using an SMTP server.
The CURL library is used to handle the SMTP request.
Keystrokes are sent via email every 100 keystrokes to avoid overwhelming the SMTP server with too many requests.

( ALternative ways methods using discord ) SSL certificate may have issues. Fixing it
## Exit Condition:

The program runs until the [ESC] key is pressed, at which point it unhooks the keyboard hook and exits.
Prerequisites
## Install CURL:

Download and install the CURL library for your platform.
Link the CURL library to your project.
## Email Configuration:

Replace the EMAIL_FROM, EMAIL_TO, SMTP_SERVER, SMTP_USERNAME, and SMTP_PASSWORD constants with your actual email and SMTP server details.
Ensure that your SMTP server supports STARTTLS for secure communication.
## Libraries:

Ensure that you have the curl library linked and configured in your project.
Important Notes
## Ethics and Legality:

You must have explicit permission to log keystrokes and send them via email.
Misusing this code can lead to legal consequences and harm to others.
## Antivirus Detection:

Keyloggers are often flagged by antivirus software as malicious. If you test this code, it may be detected and quarantined.
## Security:

Ensure that your email credentials and SMTP server details are secure and not shared with unauthorized parties.
Educational Purposes Only:

This example is provided for educational purposes to help you understand how keyloggers and SMTP requests work. Use this knowledge responsibly.
If you have any further questions or need help with specific parts of the implementation, feel free to ask!
