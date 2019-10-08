#ifndef AWS_CLIENT_CREDENTIAL_KEYS_H
#define AWS_CLIENT_CREDENTIAL_KEYS_H

#include <stdint.h>

/*
 * PEM-encoded client certificate.
 *
 * Must include the PEM header and footer:
 * "-----BEGIN CERTIFICATE-----\n"\
 * "...base64 data...\n"\
 * "-----END CERTIFICATE-----"
 */
#define keyCLIENT_CERTIFICATE_PEM \
"-----BEGIN CERTIFICATE-----\n"\
"MIIDWjCCAkKgAwIBAgIVAIjC2b5OxkXnmZqzU9YEqB+fSQdXMA0GCSqGSIb3DQEB\n"\
"CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t\n"\
"IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0xOTA3MDkwMDM5\n"\
"MjlaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh\n"\
"dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDhPyeStGrLBTTheT39\n"\
"lxNrYchSHkypLDDI2CvP/bSykXeIAVsy53oGVxK8ogzziyGkmO2r1ni9gW3x/93x\n"\
"h52nMKP2ZvBhImcWFs59hzihUSBX24j7xLUWnjViglRadn15BQui8f+W7qHqqzF4\n"\
"pgdTfBw80/HYXXYG77TN7Z/V6reGAelcRrLKXbY8UQgW3JaPHnlMvFpiKQobeCG2\n"\
"I0qbHWOvIKUR/XtBjFZ6qrMQ3tZSMWTY8qXaMRO7zIbVjyn5L46uLv5G11fgUaAK\n"\
"UdELvVAe4f9GFFqVNP9vsp2KRdz5FiJozV60GKR1mILthwyTI/h7fn4/ZmvJ4bYb\n"\
"dYC5AgMBAAGjYDBeMB8GA1UdIwQYMBaAFApwWhfcXD/RjLh8cl1AvJ8T9WxcMB0G\n"\
"A1UdDgQWBBT7YqGoxMyYRC/4um6MjR2GtoBOyjAMBgNVHRMBAf8EAjAAMA4GA1Ud\n"\
"DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAYeivDpYbHvZhDXlzVYRChg14\n"\
"1pMbMhaETDMtgi1DXSCzsOVOqZrjY+Hj6+/NttqiBlkJjRB9PWvW6buaRqPjze8P\n"\
"0vJ+v2Q0ne4O2RGuf6xFD9OcaOFOuVwJaQHslPrqWsOLmePtUykid4i6S0lJqWwp\n"\
"a1xjdmI76khesjgSjc7qBFLrZx574DJOhozkz8el4cS4FXiDijKW5N4EEnDNS6Lx\n"\
"u2ickmiuBv6iYEh7r1wbzNG+eiZ+PC58i0uM/FocnUtdtLeXsWdyn2U/2mvIPsqZ\n"\
"xRF37HnGpaXWKEFrPwc3XYouIhvL+Z3bPt5e5mRqybHJb3jJV0xQAO9KppM4zQ==\n"\
"-----END CERTIFICATE-----"

/*
 * PEM-encoded client private key.
 *
 * Must include the PEM header and footer:
 * "-----BEGIN RSA PRIVATE KEY-----\n"\
 * "...base64 data...\n"\
 * "-----END RSA PRIVATE KEY-----"
 */
#define keyCLIENT_PRIVATE_KEY_PEM \
"-----BEGIN RSA PRIVATE KEY-----\n"\
"MIIEogIBAAKCAQEA4T8nkrRqywU04Xk9/ZcTa2HIUh5MqSwwyNgrz/20spF3iAFb\n"\
"Mud6BlcSvKIM84shpJjtq9Z4vYFt8f/d8YedpzCj9mbwYSJnFhbOfYc4oVEgV9uI\n"\
"+8S1Fp41YoJUWnZ9eQULovH/lu6h6qsxeKYHU3wcPNPx2F12Bu+0ze2f1eq3hgHp\n"\
"XEayyl22PFEIFtyWjx55TLxaYikKG3ghtiNKmx1jryClEf17QYxWeqqzEN7WUjFk\n"\
"2PKl2jETu8yG1Y8p+S+Ori7+RtdX4FGgClHRC71QHuH/RhRalTT/b7KdikXc+RYi\n"\
"aM1etBikdZiC7YcMkyP4e35+P2ZryeG2G3WAuQIDAQABAoIBADFS0giXB1ejsTu6\n"\
"f++a+H6oL+SDjAfzjE8+vwFRrhL4NbOLkHvqIldnE8xItAtUqS5Q6qKZWWEuCXRX\n"\
"sjMknkoxJYZcbkDm9qnonpjwVl9E049JnIp9PaZRSKFkprIUhk9M4opw8aYMZVps\n"\
"2G1x9eK7J7fONYRKmwDT1sTNRxXhYb3jzBdKiGcLD/MMDkS9wDuREupYRfCrbB3A\n"\
"4dOQdX/HTVAAi/jIyoowWZ5aG1ebcCAc/UEE9Rrxtzx2F5XnHHuQboWNWU4KFmFa\n"\
"IBZvaXcqoPVrwvUUIzuE52s5LdiXPAe3oBcRmLt1kCkSoWF6GcZ2sELNRjNd/jGC\n"\
"UlBI8wECgYEA+1H1xfocqKdaedCV5knhBiUCSkbxIhs+lvW1qNHEffiyAo6DTmlO\n"\
"P2SwR/adFgAh5wRoiv2S8QWc8Yb8ngFkDVJLcaI4AejRdoRrkNo0vhM6q2IiKua/\n"\
"1tmvAFTW5B+0xu90eSDEVWrrFP63r7Kd7nWTRxMDkvbRb3ZPChzHVOECgYEA5XDn\n"\
"FonFuLGfe77XIgAkoPaC45gVdV/0Xy7gCcAZfuhwY/DBqHpl27/WnpEdp+FvLn7o\n"\
"4eJTvAn0XnYmDPg5smlnQqsaqlI5iHZF04asvzoC9BT7wpcb/ym+tMULZ8F+AXPv\n"\
"531XnkonYJLRJ6PPY9qXXpfZf6cNFQYXW4ziTtkCgYAltxAGDNm+KZc3TqkgLRga\n"\
"3vTwBqNrNeF3sBn54rj1GpxEK/TikIUQmGCn/Ado5M4xqAS0Twd9KhV6XYKNh7Hn\n"\
"GeF+hRSYnMLKYWbvDv/5QlN8orZz77r8WCCeER1KkMOFUywMfXFZTDlJJkyUjl6L\n"\
"CZP3h4/yvx2A96U25Ei5oQKBgGOQLcda+8Dc2z8gnkxz0XFD56KJwW4QuIOBXNj+\n"\
"UlH+3Z6QwAizLHZCdgiTIlJSgiOkELzMsNddXnyLOpHQOYFROez1QPn9+ldcM4rJ\n"\
"2zaUHun6HRHg7tY4MDhTdVKN5XcN72H9w0K+HP5MV5SjAgDrv1nd/RTm0cd4wDQL\n"\
"G71pAoGAS9ahMEI6Zq0DoufqlhrNbL2/Q9pUeG9aXxumFII4i/q9E0ht46YWMLgi\n"\
"0ajYM47jeg/nSsCb8tSUQNO3GuaM2a66USI8FhAaJ6/2lC4OYW06WkSh5lh8jsY4\n"\
"MOQMRGayo5ZhAKwMsRQJ3q2zAJ/QREmwOiH+JOsYn8Z8I11Ij4E=\n"\
"-----END RSA PRIVATE KEY-----"

/*
 * PEM-encoded Just-in-Time Registration (JITR) certificate (optional).
 *
 * If used, must include the PEM header and footer:
 * "-----BEGIN CERTIFICATE-----\n"\
 * "...base64 data...\n"\
 * "-----END CERTIFICATE-----"
 */
#define keyJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM  NULL

/* The constants above are set to const char * pointers defined in aws_dev_mode_key_provisioning.c,
 * and externed here for use in C files.  NOTE!  THIS IS DONE FOR CONVENIENCE
 * DURING AN EVALUATION PHASE AND IS NOT GOOD PRACTICE FOR PRODUCTION SYSTEMS
 * WHICH MUST STORE KEYS SECURELY. */
extern const char clientcredentialCLIENT_CERTIFICATE_PEM[];
extern const char* clientcredentialJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM;
extern const char clientcredentialCLIENT_PRIVATE_KEY_PEM[];
extern const uint32_t clientcredentialCLIENT_CERTIFICATE_LENGTH;
extern const uint32_t clientcredentialCLIENT_PRIVATE_KEY_LENGTH;

#endif /* AWS_CLIENT_CREDENTIAL_KEYS_H */
