/***************************************************************************
 *   Copyright (C) 2009 - 2010 by Simon Qian <SimonQian@SimonQian.com>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef __HTTPD_FS_H_INCLUDED__
#define __HTTPD_FS_H_INCLUDED__


const char index_html[] =    
"<!DOCTYPE html><html>"         
"<head data-gwd-animation-mode=\"proMode\">"    
"<title>main</title>"
"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">"
"<meta name=\"generator\" content=\"Google Web Designer 1.0.1.1025\">"
"<meta name=\"viewport\" content=\"width=device-width\"/>"
"</head>"
"<p><img src=\"logo2.jpg\" width=\"299\" height=\"32\"></p>"
"<p>VSFIP HTTP DEMO</p>"
"<form action=\"submit\" method=\"post\" enctype=\"application/x-www-form-urlencoded\" name=\"form\" target=\"_self\">"
"<p><input name=\"Text\" type=\"text\" value=\"TEXT\" maxlength=\"16\"></p>"
"<p><input type=\"checkbox\" name=\"checkbox\" id=\"checkbox\">CheckBox</p>"
"<p><input name=\"radio\" type=\"radio\" id=\"radio\" value=\"ON\" checked>CheckButton LEDON</p>"
"<p><input type=\"radio\" name=\"radio\" id=\"radio2\" value=\"OFF\">CheckButton LEDOFF</p>"
"<p><select name=\"select\" id=\"select\">"
"<option value=\"1\">1</option>"
"<option value=\"2\">2</option>"
"<option value=\"3\">3</option>"
"<option value=\"4\">4</option>"
"</select>Menu</p>"
"<p><input type=\"submit\" name=\"button\" id=\"button\" value=\"Submit\">Submit</p>"
"</form></body></html>";

const char login_html[] = 
"<!DOCTYPE html><html>"
"<head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />"
"<title>Login</title>"
"</head>"
"<body>"
"<form action=\"login\" method=\"post\" enctype=\"application/x-www-form-urlencoded\" name=\"main\" target=\"login\">"
"<p>VSFIP HTTPD DEMO</p>"
"<p>Please Login First</p>"
"<table width=\"300\" border=\"1\">"
"<tr><th scope=\"row\">Username</th>"
"<td><input name=\"user\" type=\"text\" value=\"username\" /></td>"
"</tr>"
"<tr><th scope=\"row\">Password</th>"
"<td><input name=\"pass\" type=\"password\" value=\"password\" /></td>"
"</tr>"
"</table>"
"<input type=\"submit\" name=\"login\" value=\"Login\" />"
"<input type=\"submit\" name=\"reg\" value=\"Regist\" />"
"</form>"
"</body>"
"</html>";

const char loginsuccess_html[] = 
"<!DOCTYPE html><html xmlns=\"http://www.w3.org/1999/xhtml\">"
"<head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />"
"<title>Login Success</title></head><body>Login Success<a href=\"index.htm\">Enter</a>"
"</body></html>";

const char loginfail_html[] = 
"<!DOCTYPE html><html xmlns=\"http://www.w3.org/1999/xhtml\">"
"<head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />"
"<title>Login Fail</title></head><body>Login Fail<a href=\"login.htm\">Return</a>"
"</body></html>";

const char webimg_logo2[] = {
	0x47, 0x49, 0x46, 0x38, 0x39, 0x61, 0xED, 0x00, 0x2A, 0x00, 0x70, 0x00, 0x00, 0x21, 0xF9, 0x04, 
	0x01, 0x00, 0x00, 0xFC, 0x00, 0x2C, 0x00, 0x00, 0x00, 0x00, 0xED, 0x00, 0x2A, 0x00, 0x87, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x33, 0x00, 0x00, 0x66, 0x00, 0x00, 0x99, 0x00, 0x00, 0xCC, 0x00, 0x00, 
	0xFF, 0x00, 0x2B, 0x00, 0x00, 0x2B, 0x33, 0x00, 0x2B, 0x66, 0x00, 0x2B, 0x99, 0x00, 0x2B, 0xCC, 
	0x00, 0x2B, 0xFF, 0x00, 0x55, 0x00, 0x00, 0x55, 0x33, 0x00, 0x55, 0x66, 0x00, 0x55, 0x99, 0x00, 
	0x55, 0xCC, 0x00, 0x55, 0xFF, 0x00, 0x80, 0x00, 0x00, 0x80, 0x33, 0x00, 0x80, 0x66, 0x00, 0x80, 
	0x99, 0x00, 0x80, 0xCC, 0x00, 0x80, 0xFF, 0x00, 0xAA, 0x00, 0x00, 0xAA, 0x33, 0x00, 0xAA, 0x66, 
	0x00, 0xAA, 0x99, 0x00, 0xAA, 0xCC, 0x00, 0xAA, 0xFF, 0x00, 0xD5, 0x00, 0x00, 0xD5, 0x33, 0x00, 
	0xD5, 0x66, 0x00, 0xD5, 0x99, 0x00, 0xD5, 0xCC, 0x00, 0xD5, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0xFF, 
	0x33, 0x00, 0xFF, 0x66, 0x00, 0xFF, 0x99, 0x00, 0xFF, 0xCC, 0x00, 0xFF, 0xFF, 0x33, 0x00, 0x00, 
	0x33, 0x00, 0x33, 0x33, 0x00, 0x66, 0x33, 0x00, 0x99, 0x33, 0x00, 0xCC, 0x33, 0x00, 0xFF, 0x33, 
	0x2B, 0x00, 0x33, 0x2B, 0x33, 0x33, 0x2B, 0x66, 0x33, 0x2B, 0x99, 0x33, 0x2B, 0xCC, 0x33, 0x2B, 
	0xFF, 0x33, 0x55, 0x00, 0x33, 0x55, 0x33, 0x33, 0x55, 0x66, 0x33, 0x55, 0x99, 0x33, 0x55, 0xCC, 
	0x33, 0x55, 0xFF, 0x33, 0x80, 0x00, 0x33, 0x80, 0x33, 0x33, 0x80, 0x66, 0x33, 0x80, 0x99, 0x33, 
	0x80, 0xCC, 0x33, 0x80, 0xFF, 0x33, 0xAA, 0x00, 0x33, 0xAA, 0x33, 0x33, 0xAA, 0x66, 0x33, 0xAA, 
	0x99, 0x33, 0xAA, 0xCC, 0x33, 0xAA, 0xFF, 0x33, 0xD5, 0x00, 0x33, 0xD5, 0x33, 0x33, 0xD5, 0x66, 
	0x33, 0xD5, 0x99, 0x33, 0xD5, 0xCC, 0x33, 0xD5, 0xFF, 0x33, 0xFF, 0x00, 0x33, 0xFF, 0x33, 0x33, 
	0xFF, 0x66, 0x33, 0xFF, 0x99, 0x33, 0xFF, 0xCC, 0x33, 0xFF, 0xFF, 0x66, 0x00, 0x00, 0x66, 0x00, 
	0x33, 0x66, 0x00, 0x66, 0x66, 0x00, 0x99, 0x66, 0x00, 0xCC, 0x66, 0x00, 0xFF, 0x66, 0x2B, 0x00, 
	0x66, 0x2B, 0x33, 0x66, 0x2B, 0x66, 0x66, 0x2B, 0x99, 0x66, 0x2B, 0xCC, 0x66, 0x2B, 0xFF, 0x66, 
	0x55, 0x00, 0x66, 0x55, 0x33, 0x66, 0x55, 0x66, 0x66, 0x55, 0x99, 0x66, 0x55, 0xCC, 0x66, 0x55, 
	0xFF, 0x66, 0x80, 0x00, 0x66, 0x80, 0x33, 0x66, 0x80, 0x66, 0x66, 0x80, 0x99, 0x66, 0x80, 0xCC, 
	0x66, 0x80, 0xFF, 0x66, 0xAA, 0x00, 0x66, 0xAA, 0x33, 0x66, 0xAA, 0x66, 0x66, 0xAA, 0x99, 0x66, 
	0xAA, 0xCC, 0x66, 0xAA, 0xFF, 0x66, 0xD5, 0x00, 0x66, 0xD5, 0x33, 0x66, 0xD5, 0x66, 0x66, 0xD5, 
	0x99, 0x66, 0xD5, 0xCC, 0x66, 0xD5, 0xFF, 0x66, 0xFF, 0x00, 0x66, 0xFF, 0x33, 0x66, 0xFF, 0x66, 
	0x66, 0xFF, 0x99, 0x66, 0xFF, 0xCC, 0x66, 0xFF, 0xFF, 0x99, 0x00, 0x00, 0x99, 0x00, 0x33, 0x99, 
	0x00, 0x66, 0x99, 0x00, 0x99, 0x99, 0x00, 0xCC, 0x99, 0x00, 0xFF, 0x99, 0x2B, 0x00, 0x99, 0x2B, 
	0x33, 0x99, 0x2B, 0x66, 0x99, 0x2B, 0x99, 0x99, 0x2B, 0xCC, 0x99, 0x2B, 0xFF, 0x99, 0x55, 0x00, 
	0x99, 0x55, 0x33, 0x99, 0x55, 0x66, 0x99, 0x55, 0x99, 0x99, 0x55, 0xCC, 0x99, 0x55, 0xFF, 0x99, 
	0x80, 0x00, 0x99, 0x80, 0x33, 0x99, 0x80, 0x66, 0x99, 0x80, 0x99, 0x99, 0x80, 0xCC, 0x99, 0x80, 
	0xFF, 0x99, 0xAA, 0x00, 0x99, 0xAA, 0x33, 0x99, 0xAA, 0x66, 0x99, 0xAA, 0x99, 0x99, 0xAA, 0xCC, 
	0x99, 0xAA, 0xFF, 0x99, 0xD5, 0x00, 0x99, 0xD5, 0x33, 0x99, 0xD5, 0x66, 0x99, 0xD5, 0x99, 0x99, 
	0xD5, 0xCC, 0x99, 0xD5, 0xFF, 0x99, 0xFF, 0x00, 0x99, 0xFF, 0x33, 0x99, 0xFF, 0x66, 0x99, 0xFF, 
	0x99, 0x99, 0xFF, 0xCC, 0x99, 0xFF, 0xFF, 0xCC, 0x00, 0x00, 0xCC, 0x00, 0x33, 0xCC, 0x00, 0x66, 
	0xCC, 0x00, 0x99, 0xCC, 0x00, 0xCC, 0xCC, 0x00, 0xFF, 0xCC, 0x2B, 0x00, 0xCC, 0x2B, 0x33, 0xCC, 
	0x2B, 0x66, 0xCC, 0x2B, 0x99, 0xCC, 0x2B, 0xCC, 0xCC, 0x2B, 0xFF, 0xCC, 0x55, 0x00, 0xCC, 0x55, 
	0x33, 0xCC, 0x55, 0x66, 0xCC, 0x55, 0x99, 0xCC, 0x55, 0xCC, 0xCC, 0x55, 0xFF, 0xCC, 0x80, 0x00, 
	0xCC, 0x80, 0x33, 0xCC, 0x80, 0x66, 0xCC, 0x80, 0x99, 0xCC, 0x80, 0xCC, 0xCC, 0x80, 0xFF, 0xCC, 
	0xAA, 0x00, 0xCC, 0xAA, 0x33, 0xCC, 0xAA, 0x66, 0xCC, 0xAA, 0x99, 0xCC, 0xAA, 0xCC, 0xCC, 0xAA, 
	0xFF, 0xCC, 0xD5, 0x00, 0xCC, 0xD5, 0x33, 0xCC, 0xD5, 0x66, 0xCC, 0xD5, 0x99, 0xCC, 0xD5, 0xCC, 
	0xCC, 0xD5, 0xFF, 0xCC, 0xFF, 0x00, 0xCC, 0xFF, 0x33, 0xCC, 0xFF, 0x66, 0xCC, 0xFF, 0x99, 0xCC, 
	0xFF, 0xCC, 0xCC, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x33, 0xFF, 0x00, 0x66, 0xFF, 0x00, 
	0x99, 0xFF, 0x00, 0xCC, 0xFF, 0x00, 0xFF, 0xFF, 0x2B, 0x00, 0xFF, 0x2B, 0x33, 0xFF, 0x2B, 0x66, 
	0xFF, 0x2B, 0x99, 0xFF, 0x2B, 0xCC, 0xFF, 0x2B, 0xFF, 0xFF, 0x55, 0x00, 0xFF, 0x55, 0x33, 0xFF, 
	0x55, 0x66, 0xFF, 0x55, 0x99, 0xFF, 0x55, 0xCC, 0xFF, 0x55, 0xFF, 0xFF, 0x80, 0x00, 0xFF, 0x80, 
	0x33, 0xFF, 0x80, 0x66, 0xFF, 0x80, 0x99, 0xFF, 0x80, 0xCC, 0xFF, 0x80, 0xFF, 0xFF, 0xAA, 0x00, 
	0xFF, 0xAA, 0x33, 0xFF, 0xAA, 0x66, 0xFF, 0xAA, 0x99, 0xFF, 0xAA, 0xCC, 0xFF, 0xAA, 0xFF, 0xFF, 
	0xD5, 0x00, 0xFF, 0xD5, 0x33, 0xFF, 0xD5, 0x66, 0xFF, 0xD5, 0x99, 0xFF, 0xD5, 0xCC, 0xFF, 0xD5, 
	0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x33, 0xFF, 0xFF, 0x66, 0xFF, 0xFF, 0x99, 0xFF, 0xFF, 0xCC, 
	0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 
	0xFF, 0x00, 0xF7, 0x09, 0x1C, 0x48, 0xB0, 0xA0, 0xC1, 0x83, 0x08, 0x13, 0x2A, 0x5C, 0xC8, 0xB0, 
	0xA1, 0xC3, 0x87, 0x10, 0x23, 0x4A, 0x9C, 0x48, 0xB1, 0xA2, 0xC5, 0x8B, 0x18, 0x33, 0x6A, 0xDC, 
	0xC8, 0xB1, 0xA3, 0xC7, 0x8F, 0x20, 0x43, 0x8A, 0x1C, 0x49, 0xB2, 0xA4, 0xC9, 0x93, 0x28, 0x53, 
	0xAA, 0x5C, 0xC9, 0xB2, 0xA5, 0xCB, 0x97, 0x30, 0x5F, 0xEA, 0x33, 0x39, 0x73, 0x5F, 0xCD, 0x8D, 
	0x37, 0x63, 0x0A, 0xCC, 0x39, 0xB1, 0x26, 0x4F, 0x8D, 0x3F, 0x2D, 0xCE, 0x1C, 0x6A, 0xB3, 0x28, 
	0xD1, 0x82, 0x44, 0x93, 0x1A, 0x5D, 0x7A, 0xD4, 0xA0, 0x52, 0x82, 0x47, 0x83, 0xF6, 0x2C, 0xCA, 
	0x74, 0xE9, 0xCE, 0x81, 0x4A, 0xA3, 0x26, 0xCC, 0x7A, 0xF5, 0xAA, 0xCF, 0xAA, 0x5C, 0xB5, 0x52, 
	0x05, 0x4B, 0x96, 0x6A, 0x58, 0xAF, 0x63, 0x2B, 0x9E, 0xC5, 0x4A, 0xF6, 0xEC, 0xD3, 0xAF, 0x68, 
	0xAB, 0x5A, 0x95, 0xEA, 0xF0, 0xAB, 0x3E, 0xB8, 0x70, 0xD9, 0x96, 0x3D, 0xA8, 0xF5, 0x6D, 0xDB, 
	0xB2, 0x4D, 0xD3, 0xCE, 0x95, 0x8B, 0x17, 0x6D, 0xD2, 0xC0, 0x3D, 0x97, 0xE9, 0x03, 0x10, 0x43, 
	0x85, 0x18, 0x49, 0x92, 0xD0, 0x88, 0x09, 0x23, 0xC6, 0xEA, 0xB2, 0x7D, 0x30, 0x00, 0xC0, 0x40, 
	0x33, 0x09, 0x32, 0x9A, 0x18, 0x8D, 0x61, 0xC4, 0x08, 0x83, 0x86, 0x1E, 0x66, 0xD0, 0x92, 0x88, 
	0xE5, 0xA4, 0x07, 0x6D, 0x9F, 0x8A, 0x1B, 0x92, 0x32, 0x65, 0xB2, 0x59, 0x0F, 0x5A, 0x3D, 0x7D, 
	0x31, 0x44, 0xDF, 0xB8, 0x51, 0x19, 0x31, 0xC3, 0x99, 0x97, 0x35, 0xC7, 0x10, 0x93, 0x69, 0x12, 
	0x26, 0x31, 0x37, 0x74, 0xD7, 0x54, 0x36, 0xB3, 0xF6, 0x0A, 0x00, 0x06, 0x54, 0xA0, 0x9E, 0x94, 
	0x49, 0x12, 0x72, 0xBB, 0xFB, 0x6A, 0x9F, 0x16, 0x1D, 0x26, 0xCD, 0x4C, 0x31, 0xFA, 0x14, 0xE3, 
	0xFF, 0x06, 0xCD, 0xFB, 0x06, 0xE9, 0xC8, 0x62, 0x24, 0x97, 0x8E, 0xA6, 0x2F, 0x73, 0x0C, 0x34, 
	0xB2, 0x6D, 0x5E, 0xDE, 0x17, 0x9D, 0x34, 0x9A, 0xDB, 0x06, 0x44, 0x3F, 0xCE, 0x24, 0x59, 0x74, 
	0x8C, 0xA4, 0xB5, 0x89, 0xA6, 0x1E, 0x23, 0x68, 0x84, 0xD1, 0xD8, 0x01, 0xDC, 0x4D, 0xE2, 0x9B, 
	0x42, 0x97, 0x29, 0x43, 0x0F, 0x7B, 0xF4, 0xBC, 0x77, 0x54, 0x0C, 0xB0, 0xED, 0xC3, 0xDC, 0x85, 
	0x33, 0x0D, 0x77, 0x57, 0x34, 0xF4, 0xE8, 0xD3, 0x9A, 0x3E, 0x2B, 0x98, 0x67, 0x53, 0x86, 0xB2, 
	0xCD, 0x44, 0x8F, 0x62, 0x8A, 0xD1, 0x63, 0x40, 0x0C, 0xB3, 0xCD, 0x64, 0xDB, 0x5D, 0x93, 0x8D, 
	0x88, 0x19, 0x1A, 0x82, 0x31, 0xB4, 0x4C, 0x6D, 0xAC, 0x99, 0xB6, 0x99, 0x4D, 0xCA, 0x14, 0x15, 
	0xA1, 0x84, 0xF2, 0xCD, 0xD4, 0xA3, 0x4D, 0xD0, 0x6C, 0x81, 0x86, 0x6F, 0x1C, 0x42, 0xA3, 0x4F, 
	0x3D, 0xFF, 0x29, 0x66, 0x13, 0x65, 0x37, 0x1C, 0xD9, 0xE1, 0x32, 0xE6, 0x31, 0xB7, 0xE4, 0x16, 
	0x92, 0x8C, 0x88, 0x5B, 0x18, 0x36, 0x75, 0xB8, 0x4F, 0x0C, 0x59, 0x9A, 0x98, 0xE4, 0x6D, 0x37, 
	0xDC, 0xC6, 0xDC, 0x6D, 0xEF, 0x59, 0xE8, 0x13, 0x68, 0xA5, 0x59, 0x68, 0x1B, 0x73, 0x2B, 0x84, 
	0x71, 0x97, 0x4D, 0xEC, 0xCD, 0x04, 0x40, 0x94, 0x13, 0xBD, 0xA9, 0x8C, 0x83, 0xD1, 0xC0, 0x00, 
	0x9E, 0x83, 0x56, 0x7E, 0x99, 0xA5, 0x9E, 0xCC, 0x49, 0x78, 0x21, 0xA0, 0xCC, 0xC1, 0x10, 0x06, 
	0xA0, 0xED, 0x49, 0x32, 0x09, 0x9D, 0xF4, 0xEC, 0xC9, 0x1C, 0x00, 0x5B, 0x64, 0x22, 0x64, 0xA4, 
	0xFA, 0x10, 0x77, 0xE1, 0x3E, 0xA6, 0xED, 0xC3, 0x65, 0x57, 0x0C, 0x41, 0x73, 0xA2, 0xA4, 0xB8, 
	0x49, 0x22, 0xAA, 0xA8, 0x7B, 0x16, 0x55, 0xA1, 0x69, 0x7B, 0x46, 0xEA, 0xE0, 0x3E, 0x00, 0x70, 
	0xFF, 0x19, 0x58, 0x8F, 0xA6, 0x0D, 0x77, 0xAA, 0xAB, 0x36, 0xC5, 0xB0, 0xC5, 0x85, 0xF4, 0x2C, 
	0xAA, 0x8F, 0x83, 0xFA, 0xDC, 0x30, 0xC9, 0xA6, 0xC1, 0xBA, 0xAA, 0xE4, 0x8E, 0x41, 0xEE, 0x99, 
	0x61, 0x8E, 0xA9, 0x6E, 0xD6, 0xA1, 0xAB, 0x73, 0xDE, 0x60, 0x29, 0xA6, 0x93, 0xDE, 0xD0, 0x61, 
	0xAB, 0xCA, 0x9E, 0xB6, 0x20, 0x42, 0xF5, 0x48, 0xEA, 0x2D, 0x0C, 0x92, 0xFC, 0xFA, 0xE6, 0x50, 
	0xB0, 0xB9, 0xEA, 0x2A, 0x0C, 0x93, 0xB0, 0xE6, 0x2D, 0xA0, 0xD0, 0x8C, 0x96, 0x4C, 0x87, 0x2B, 
	0xA0, 0x81, 0xC9, 0x9C, 0x7B, 0x0E, 0x33, 0x69, 0x18, 0xD3, 0xEA, 0xC3, 0x21, 0x98, 0xBC, 0xF6, 
	0x38, 0x13, 0x69, 0x74, 0x19, 0xD4, 0xED, 0x32, 0x92, 0xBE, 0x1B, 0x83, 0x82, 0xA4, 0x2A, 0x0B, 
	0xE2, 0x91, 0x1C, 0xEE, 0x49, 0x8C, 0x32, 0xC9, 0xEC, 0xA9, 0x28, 0x73, 0x03, 0x3D, 0xC8, 0xEA, 
	0xAF, 0x47, 0x3A, 0x8C, 0xAD, 0x87, 0xFF, 0x5D, 0x28, 0x89, 0x32, 0xF6, 0xEE, 0x19, 0x06, 0x26, 
	0xD4, 0x46, 0xB8, 0x68, 0xA4, 0x11, 0x86, 0x6B, 0xB1, 0x32, 0x8A, 0x39, 0x68, 0x6D, 0xC1, 0x29, 
	0x9F, 0xEA, 0x60, 0x8A, 0x60, 0x66, 0x52, 0xEF, 0xAF, 0xAF, 0xFD, 0xEA, 0xE0, 0xC3, 0x0A, 0x8F, 
	0x36, 0x91, 0xC3, 0xF6, 0xD2, 0xE3, 0x27, 0xAA, 0x1B, 0xEF, 0xF3, 0x27, 0xC4, 0x89, 0x4A, 0x19, 
	0xE9, 0xC3, 0x21, 0xEF, 0x19, 0x43, 0xC4, 0xD0, 0x6C, 0x56, 0x99, 0xAB, 0xF6, 0x46, 0x9C, 0x9B, 
	0xCD, 0xBF, 0x46, 0xEA, 0xEB, 0x9E, 0x38, 0x52, 0x2C, 0xC6, 0x6D, 0x0F, 0xE9, 0x4C, 0x8C, 0xBD, 
	0xED, 0x95, 0xA6, 0x8F, 0xAB, 0x80, 0x3E, 0x99, 0x89, 0x99, 0x7B, 0x66, 0x42, 0x4C, 0x32, 0x8B, 
	0xFD, 0x27, 0xA4, 0x40, 0x17, 0x2B, 0x43, 0xD9, 0xD9, 0x68, 0x4B, 0x2A, 0x74, 0x0C, 0x4E, 0x47, 
	0xFF, 0x8C, 0x74, 0x7B, 0x08, 0x5F, 0x08, 0xCD, 0xA2, 0x11, 0xEB, 0x63, 0x40, 0x6F, 0x3D, 0x4A, 
	0xAA, 0xD8, 0x16, 0xD0, 0xD4, 0xCB, 0x9A, 0x9F, 0x59, 0x7B, 0x7B, 0x17, 0x7C, 0x68, 0x2F, 0x06, 
	0x1E, 0xDA, 0x11, 0x47, 0x2A, 0x10, 0x0C, 0x35, 0x26, 0xA4, 0x4C, 0xB7, 0x6F, 0x43, 0x0C, 0x8D, 
	0x0A, 0xE1, 0xAE, 0xFB, 0x2B, 0x6E, 0xF6, 0x86, 0xAE, 0x0C, 0x98, 0x6E, 0xD6, 0x2B, 0xA9, 0xBD, 
	0xC3, 0x04, 0xCD, 0x79, 0x18, 0xC9, 0xD8, 0x34, 0x4C, 0x26, 0xC9, 0x10, 0x03, 0x03, 0x6E, 0xD3, 
	0x2A, 0x33, 0xF8, 0x9F, 0x68, 0x3B, 0xD9, 0x5E, 0x8B, 0x0C, 0x71, 0x48, 0x30, 0x31, 0x9F, 0x20, 
	0x8D, 0xAE, 0xCE, 0x1A, 0x5F, 0xC8, 0xA2, 0x90, 0xCA, 0x30, 0x2D, 0xEA, 0xF2, 0x54, 0x29, 0x13, 
	0x0D, 0xA0, 0x1A, 0x26, 0xB9, 0xEE, 0x9E, 0xED, 0x76, 0x18, 0x43, 0xA4, 0xC3, 0x44, 0x9C, 0x4C, 
	0x0C, 0x21, 0x47, 0xCE, 0xB7, 0x32, 0xF3, 0x8C, 0x4E, 0xE3, 0x92, 0xD8, 0xB2, 0xB6, 0xA8, 0xEA, 
	0xCA, 0xAC, 0x80, 0x09, 0xAA, 0x0F, 0x13, 0xD3, 0x38, 0x3D, 0x62, 0x84, 0x9C, 0xBB, 0xFA, 0xBF, 
	0xEE, 0xC3, 0xF3, 0xC3, 0xFD, 0x9B, 0xC8, 0xF5, 0xA2, 0x67, 0x33, 0x15, 0x4D, 0x42, 0x54, 0xCC, 
	0x03, 0xD6, 0x7B, 0x22, 0x55, 0xB8, 0xE5, 0x85, 0x07, 0x6D, 0x54, 0x53, 0x46, 0x26, 0x86, 0x01, 
	0x0D, 0x00, 0x78, 0x2A, 0x13, 0xAE, 0x22, 0x86, 0xCD, 0x00, 0xD0, 0x2B, 0x49, 0x30, 0x8B, 0x34, 
	0xC9, 0xD2, 0x5B, 0x34, 0x54, 0x70, 0x24, 0x88, 0xB8, 0x4E, 0x35, 0xAC, 0xE3, 0x11, 0xC4, 0x1E, 
	0xA6, 0xA3, 0xD9, 0x5C, 0x8B, 0x18, 0xA0, 0x08, 0x99, 0xB4, 0xC0, 0xC6, 0xA1, 0x86, 0xF5, 0x68, 
	0x33, 0x67, 0xB3, 0x61, 0xF4, 0xDA, 0xD7, 0x38, 0x49, 0xB9, 0xCD, 0x41, 0x30, 0xA0, 0x60, 0xDE, 
	0xFF, 0x86, 0x31, 0xB6, 0x1B, 0x86, 0xA9, 0x35, 0x92, 0xE2, 0x90, 0x18, 0x90, 0x46, 0xB6, 0xE5, 
	0x79, 0x2B, 0x68, 0xFA, 0xA0, 0x1C, 0xC4, 0x22, 0xB5, 0xA3, 0xBB, 0x6C, 0xCF, 0x2A, 0x0F, 0x71, 
	0x95, 0xCD, 0xE0, 0x84, 0x09, 0x4E, 0xB1, 0x6C, 0x87, 0x2F, 0xC2, 0x17, 0xB6, 0x9E, 0xC7, 0x29, 
	0xA2, 0x7D, 0x8C, 0x80, 0xC3, 0x48, 0x9E, 0x0A, 0x82, 0x35, 0x0C, 0x0E, 0x25, 0x63, 0x82, 0xCA, 
	0x30, 0x00, 0x6E, 0x48, 0xC6, 0x1C, 0x7A, 0x48, 0x49, 0x85, 0xE6, 0xB2, 0xC9, 0x0D, 0x40, 0xA5, 
	0x10, 0x8D, 0x85, 0x0C, 0x06, 0x2D, 0x92, 0x14, 0xCF, 0x94, 0x74, 0x30, 0xE8, 0x49, 0xB0, 0x55, 
	0x95, 0xAA, 0x9E, 0x85, 0x00, 0x15, 0x86, 0x2C, 0x85, 0xD0, 0x5B, 0xCB, 0x38, 0xD1, 0xA8, 0x54, 
	0x17, 0x21, 0x4B, 0x1D, 0xEA, 0x75, 0xA2, 0x0A, 0x40, 0x98, 0xBA, 0x65, 0xB1, 0x0E, 0x2D, 0x0A, 
	0x64, 0x92, 0x62, 0x1D, 0xFD, 0x44, 0x67, 0x47, 0x4C, 0x80, 0x12, 0x6E, 0x2B, 0x08, 0x97, 0x85, 
	0x2C, 0xC4, 0x9A, 0xCA, 0x0C, 0x49, 0x22, 0x04, 0x03, 0xA5, 0xEF, 0x20, 0xA7, 0x42, 0x6F, 0xD1, 
	0xE3, 0x06, 0xF6, 0x9A, 0x47, 0xA2, 0x7A, 0x43, 0xA7, 0x99, 0x04, 0x6E, 0x18, 0x9A, 0xB0, 0x97, 
	0x05, 0x01, 0xA9, 0xB3, 0x64, 0x08, 0xF3, 0x96, 0x1E, 0x7C, 0x86, 0x1D, 0x15, 0x44, 0x95, 0xE3, 
	0x71, 0x0F, 0x44, 0x61, 0x72, 0xC8, 0x9E, 0x34, 0xA1, 0x2C, 0x00, 0x3C, 0x6A, 0x28, 0x0E, 0xCB, 
	0x1C, 0x6C, 0x4C, 0x04, 0xB4, 0x07, 0x51, 0x4F, 0x5F, 0x4B, 0xF3, 0x9D, 0x0A, 0x70, 0xD0, 0x14, 
	0xC5, 0x70, 0x32, 0x92, 0xAD, 0x51, 0x61, 0x30, 0x6F, 0xE8, 0xC2, 0x07, 0x36, 0xAF, 0x6C, 0x43, 
	0x69, 0xD8, 0xF5, 0xA2, 0x21, 0x06, 0x57, 0x65, 0x0E, 0x87, 0x1E, 0xDA, 0x93, 0x36, 0x19, 0xC1, 
	0xFF, 0xC0, 0x6A, 0xAE, 0x8F, 0x53, 0x73, 0x62, 0x44, 0xC0, 0x16, 0x32, 0xC9, 0xCC, 0x1D, 0x20, 
	0x4C, 0xB1, 0xE4, 0x19, 0x6B, 0x2A, 0x05, 0x37, 0xE6, 0xA8, 0xA0, 0x8B, 0xD7, 0x2A, 0xA3, 0x25, 
	0x25, 0x78, 0x3B, 0x65, 0xC0, 0x20, 0x6A, 0x93, 0x08, 0x99, 0xBD, 0x0C, 0xE0, 0x3D, 0x53, 0xB9, 
	0x8F, 0x78, 0x5D, 0xD3, 0x1B, 0x7D, 0xAE, 0xC9, 0x90, 0x95, 0x95, 0x0F, 0x00, 0x5D, 0x74, 0x27, 
	0xD2, 0x6E, 0xF9, 0x29, 0x24, 0x4A, 0x4E, 0x05, 0x2D, 0xAA, 0x4D, 0x2C, 0xDF, 0x45, 0xCB, 0x82, 
	0x28, 0x26, 0x1A, 0xB7, 0xC1, 0xA9, 0x85, 0x22, 0x19, 0xBD, 0xC2, 0x01, 0xC0, 0x52, 0x1C, 0x5A, 
	0x24, 0xF2, 0x58, 0x88, 0x2E, 0xDA, 0x84, 0xE7, 0x55, 0xBD, 0x6A, 0x5C, 0xE8, 0xA0, 0x91, 0x4A, 
	0xBA, 0xAD, 0x4B, 0x8C, 0xFA, 0x84, 0xD3, 0x91, 0xE6, 0xC6, 0x1C, 0x66, 0x0E, 0xD4, 0x73, 0x87, 
	0x2C, 0x94, 0x2A, 0xA1, 0xD7, 0xB4, 0x72, 0x41, 0x0C, 0x37, 0x4B, 0x0C, 0xCF, 0xD9, 0x32, 0x85, 
	0x41, 0x06, 0xAE, 0x13, 0x00, 0x30, 0x6A, 0xDA, 0x30, 0x60, 0xE0, 0x20, 0x1B, 0x7C, 0xEC, 0x6C, 
	0x8E, 0xCA, 0x49, 0x3D, 0x62, 0xD9, 0xAC, 0x12, 0x36, 0xA4, 0x5B, 0x3D, 0xED, 0x51, 0xAC, 0x8A, 
	0x22, 0x2A, 0xBF, 0x6D, 0xC1, 0x3C, 0x26, 0x5A, 0xE4, 0x3A, 0xA3, 0xE6, 0x1D, 0xA7, 0xE6, 0xCE, 
	0x77, 0x1A, 0x1A, 0x4B, 0x4D, 0x4C, 0x13, 0xD4, 0x81, 0xB4, 0xAD, 0x59, 0xBD, 0xD1, 0x17, 0x5E, 
	0xDF, 0xC6, 0x9A, 0x34, 0x81, 0x33, 0x55, 0xCC, 0x59, 0x22, 0xD2, 0x1C, 0x74, 0xB8, 0xB1, 0xA2, 
	0x4F, 0x19, 0xB1, 0xF9, 0x58, 0x06, 0x27, 0x75, 0xB9, 0xD3, 0x05, 0x8B, 0x99, 0xAB, 0x84, 0x88, 
	0x0D, 0x1F, 0xF6, 0xAE, 0x83, 0x16, 0xE5, 0x8B, 0x2C, 0x5C, 0x1D, 0xEE, 0x1C, 0x14, 0xB5, 0xFC, 
	0xFF, 0x61, 0xC2, 0x3A, 0x37, 0xB8, 0x9A, 0xBE, 0x40, 0xA6, 0x41, 0x65, 0xA0, 0x95, 0x8D, 0xFA, 
	0x10, 0x1F, 0x47, 0x8B, 0x84, 0x86, 0x79, 0x60, 0xCC, 0xAA, 0xFB, 0x20, 0x1A, 0x0B, 0x17, 0x13, 
	0x4D, 0xA7, 0x10, 0x64, 0xB5, 0x53, 0x64, 0xCC, 0x5D, 0xF2, 0x16, 0x06, 0x18, 0x94, 0xC9, 0xB1, 
	0xD9, 0x74, 0x10, 0xEB, 0x76, 0xBB, 0x43, 0x20, 0xFE, 0xD3, 0x20, 0xD1, 0x28, 0x08, 0x6B, 0x8E, 
	0xE7, 0x37, 0x00, 0xA8, 0x52, 0x3E, 0x3E, 0x74, 0x28, 0xF1, 0x12, 0xC7, 0xA7, 0x30, 0x2C, 0xAD, 
	0x70, 0x8E, 0x89, 0x4C, 0x75, 0xF6, 0x43, 0x2D, 0x90, 0xF9, 0x4D, 0x51, 0xD5, 0x49, 0x83, 0x18, 
	0x72, 0x93, 0xC2, 0x8A, 0x10, 0x2C, 0x64, 0x7D, 0x42, 0x6E, 0xDB, 0xA0, 0xA6, 0x85, 0xD2, 0x34, 
	0x30, 0xAC, 0x18, 0xF2, 0xD9, 0xD2, 0x3E, 0x61, 0x2F, 0xB6, 0x46, 0x88, 0x8E, 0xB9, 0xBB, 0x41, 
	0xC4, 0xC2, 0xE0, 0x2B, 0x7D, 0xE0, 0x2B, 0x2D, 0x3B, 0x8C, 0x18, 0x66, 0x78, 0x59, 0xBC, 0xE8, 
	0xC5, 0x90, 0x39, 0x06, 0x30, 0xEF, 0x04, 0x33, 0x41, 0xE1, 0xEA, 0x72, 0x6A, 0x29, 0xEF, 0x34, 
	0x00, 0x33, 0x59, 0xC5, 0xDA, 0xD5, 0x31, 0x42, 0x30, 0xF5, 0xE0, 0x56, 0x8A, 0xC1, 0x53, 0x3D, 
	0x59, 0x96, 0x2A, 0xB0, 0x92, 0xB2, 0x89, 0x66, 0x41, 0x91, 0xBB, 0x11, 0xDE, 0x60, 0xBA, 0x16, 
	0x9E, 0xEA, 0xD9, 0xA0, 0x31, 0x54, 0x07, 0xC5, 0x0A, 0x81, 0x14, 0x7B, 0x8D, 0x59, 0xC2, 0x86, 
	0xD7, 0xBC, 0x0A, 0x6D, 0x7D, 0xD0, 0xDD, 0x93, 0x0A, 0xD8, 0xFA, 0x2E, 0xDF, 0x86, 0x81, 0x68, 
	0xFD, 0xEB, 0x18, 0xD0, 0x92, 0x81, 0xD6, 0xA8, 0xA1, 0xC1, 0x41, 0xC9, 0x90, 0x44, 0x95, 0xC1, 
	0xF0, 0x27, 0x0B, 0xA3, 0x36, 0x3B, 0x3D, 0x54, 0x28, 0x00, 0x68, 0xC4, 0xC7, 0x83, 0x50, 0x0D, 
	0xFF, 0x14, 0x7B, 0x92, 0x6E, 0x02, 0x55, 0xE3, 0x2F, 0xC7, 0x46, 0x6A, 0x8B, 0xF4, 0x90, 0x1F, 
	0x91, 0xDE, 0x69, 0xDE, 0x36, 0x1F, 0x64, 0xB5, 0x99, 0x23, 0x61, 0x51, 0x9A, 0xCC, 0x5A, 0x03, 
	0x42, 0xAA, 0x86, 0x91, 0x52, 0x81, 0xA8, 0xAA, 0x66, 0xE5, 0x48, 0x89, 0x0A, 0x4C, 0xFD, 0x13, 
	0x64, 0x01, 0x0F, 0x07, 0x41, 0x42, 0xFA, 0xB9, 0x21, 0x0D, 0x2B, 0x60, 0xBB, 0x9A, 0x9B, 0x5D, 
	0x15, 0xA9, 0x40, 0x7C, 0x71, 0xCB, 0x9B, 0xF7, 0x78, 0x94, 0x8C, 0x18, 0x82, 0x42, 0x05, 0xAF, 
	0x0A, 0x43, 0xC8, 0x3E, 0x36, 0x61, 0x34, 0xBC, 0xCB, 0xC2, 0x85, 0xAD, 0x09, 0x5E, 0x35, 0x71, 
	0xE0, 0xCE, 0x1D, 0xA4, 0x6D, 0x63, 0xD3, 0x87, 0x0A, 0x02, 0x10, 0x39, 0xB4, 0x9D, 0x78, 0x20, 
	0x03, 0xA4, 0x2C, 0x61, 0x9B, 0xF3, 0xBA, 0xA4, 0xD9, 0xDA, 0x29, 0xFF, 0x4D, 0xD5, 0xE1, 0xBA, 
	0xF2, 0xD8, 0x42, 0x95, 0x70, 0x65, 0x14, 0x03, 0x80, 0x0F, 0x1B, 0xCC, 0xB7, 0xFB, 0x56, 0xE8, 
	0xB2, 0x6E, 0x83, 0x1B, 0x09, 0xCF, 0xE6, 0x37, 0xD3, 0xAC, 0xB1, 0x22, 0x93, 0x9D, 0xE2, 0x76, 
	0x77, 0x2A, 0x4B, 0x65, 0xF0, 0x86, 0x1E, 0x99, 0x33, 0x80, 0x9C, 0x32, 0xD7, 0xAC, 0xEC, 0xF8, 
	0x91, 0xA3, 0x70, 0x8B, 0x01, 0x1D, 0x4D, 0xA9, 0x4B, 0x0A, 0x9F, 0x6E, 0xDC, 0x02, 0x49, 0x73, 
	0x35, 0x39, 0xAC, 0x10, 0x42, 0xFB, 0xED, 0x06, 0x24, 0xEB, 0x61, 0xAB, 0xC2, 0x43, 0x10, 0x56, 
	0x29, 0x83, 0x9A, 0x70, 0x3A, 0x62, 0xF3, 0xE8, 0xD1, 0xE7, 0x80, 0xC5, 0x58, 0x20, 0x82, 0x84, 
	0xEF, 0x77, 0x3F, 0xE7, 0xE4, 0xA2, 0x9A, 0x88, 0x60, 0xAB, 0xFC, 0x0F, 0xFC, 0x54, 0x70, 0x65, 
	0x74, 0x17, 0xCA, 0x52, 0x67, 0x73, 0xD8, 0x87, 0xC1, 0x25, 0x70, 0xD6, 0xE8, 0xE3, 0x00, 0x16, 
	0xFF, 0x19, 0x20, 0xAD, 0xAD, 0xA7, 0xE4, 0xE5, 0x1C, 0x3C, 0x73, 0x61, 0x38, 0x83, 0x3E, 0xC1, 
	0x8A, 0x28, 0xCD, 0xC5, 0x98, 0x68, 0x0D, 0x56, 0x16, 0x98, 0x16, 0x8D, 0x34, 0xCA, 0xBC, 0x9A, 
	0x37, 0x47, 0x29, 0x68, 0xA1, 0xE4, 0xF4, 0x10, 0x5C, 0xB7, 0xDA, 0x4C, 0xA7, 0xBB, 0x4C, 0x4E, 
	0x70, 0x5A, 0x6C, 0xC3, 0xAD, 0xB8, 0x5B, 0x2B, 0x87, 0x15, 0x39, 0xF5, 0x52, 0x10, 0xF6, 0x60, 
	0x97, 0x67, 0x23, 0xFC, 0xEE, 0x22, 0xB7, 0xA8, 0x8F, 0x00, 0xEC, 0x2E, 0x84, 0x64, 0xD2, 0x67, 
	0xE8, 0xC0, 0xA5, 0x3A, 0xE6, 0x26, 0x57, 0xA8, 0xF6, 0x6A, 0x57, 0x69, 0xF2, 0x06, 0x36, 0x88, 
	0x47, 0xA4, 0x1E, 0x11, 0x9C, 0x54, 0x0C, 0x76, 0x42, 0x71, 0xF1, 0xD9, 0xAD, 0x9F, 0x2A, 0x0A, 
	0xD3, 0x9E, 0xC2, 0x2B, 0x56, 0x44, 0x41, 0xEC, 0x69, 0x40, 0xAC, 0x67, 0x2E, 0x21, 0x16, 0xF3, 
	0xD3, 0xDD, 0x92, 0x20, 0xEC, 0x95, 0x60, 0x65, 0x2F, 0x5D, 0x10, 0x3D, 0x21, 0xCF, 0x41, 0x61, 
	0x40, 0xD8, 0x2A, 0x63, 0x1C, 0xDE, 0x82, 0x08, 0xB2, 0x55, 0xDB, 0xBD, 0xDE, 0xDB, 0xEC, 0x97, 
	0x0C, 0xD1, 0x74, 0x69, 0x20, 0xF3, 0x69, 0xCA, 0x51, 0xF3, 0x6A, 0xF6, 0xE5, 0x90, 0xB7, 0x5A, 
	0xAF, 0xBD, 0x58, 0x84, 0x7E, 0xA5, 0x3A, 0x74, 0x41, 0xEC, 0x8F, 0x2C, 0x2A, 0xE3, 0xE3, 0x81, 
	0xA8, 0x20, 0x54, 0xDD, 0x9C, 0xF1, 0x0B, 0x09, 0x6F, 0x76, 0x75, 0x2D, 0x37, 0x88, 0x77, 0x97, 
	0xC2, 0x76, 0x6F, 0xCF, 0xE5, 0xD4, 0x74, 0x99, 0x4E, 0x45, 0x70, 0x57, 0x11, 0xB3, 0xAE, 0x98, 
	0xC5, 0x87, 0xCC, 0xB4, 0xD1, 0x92, 0xD8, 0xDF, 0xFA, 0x8F, 0x09, 0x37, 0xDF, 0xEA, 0xCF, 0xA7, 
	0x13, 0x21, 0xF4, 0x80, 0xA1, 0x10, 0x61, 0x10, 0xEB, 0x88, 0xA3, 0x89, 0xCD, 0x07, 0x79, 0xF8, 
	0xFF, 0x1E, 0x21, 0xAE, 0x7D, 0x67, 0x13, 0xA5, 0x5B, 0x87, 0x95, 0xBB, 0x9A, 0x16, 0xE9, 0xAF, 
	0xEF, 0x65, 0xD5, 0xA2, 0x67, 0xCC, 0x1C, 0xE0, 0xAC, 0xC8, 0xDB, 0x64, 0x60, 0xF4, 0x2E, 0x28, 
	0x5A, 0xD2, 0x45, 0x46, 0x45, 0xAA, 0xBD, 0xF6, 0x7D, 0x8A, 0xE6, 0xF6, 0x49, 0x80, 0x62, 0x4D, 
	0x83, 0xC6, 0x77, 0xBA, 0x97, 0x29, 0x92, 0x42, 0x3B, 0x9F, 0x05, 0x2E, 0x18, 0xE4, 0x3B, 0xCA, 
	0x80, 0x25, 0x08, 0xD4, 0x1E, 0xB2, 0xA2, 0x7B, 0xF5, 0xF2, 0x68, 0x44, 0x77, 0x57, 0x3D, 0xB4, 
	0x72, 0x60, 0x75, 0x55, 0xEC, 0x57, 0x3E, 0x7A, 0xC6, 0x3E, 0x20, 0x03, 0x67, 0xAC, 0xB1, 0x6D, 
	0x7C, 0xE5, 0x58, 0x45, 0xC1, 0x25, 0xB5, 0xB1, 0x70, 0x2A, 0x56, 0x3D, 0xC9, 0x66, 0x64, 0x9C, 
	0x73, 0x29, 0x89, 0x83, 0x19, 0x03, 0x54, 0x38, 0xF2, 0x83, 0x34, 0x1A, 0x76, 0x64, 0xEC, 0xD7, 
	0x2A, 0x01, 0x00, 0x51, 0xFB, 0x50, 0x43, 0x81, 0xA5, 0x0F, 0x85, 0x15, 0x11, 0x46, 0xB7, 0x61, 
	0x74, 0x87, 0x36, 0x14, 0xB6, 0x33, 0x0D, 0x04, 0x65, 0x81, 0x05, 0x77, 0x18, 0xA3, 0x4F, 0xA9, 
	0x51, 0x38, 0xA3, 0x21, 0x5A, 0xEF, 0x22, 0x2C, 0x0D, 0xD5, 0x23, 0xBC, 0x46, 0x37, 0x5E, 0xA4, 
	0x73, 0x79, 0xD1, 0x6F, 0x0B, 0xC7, 0x1B, 0x58, 0x84, 0x10, 0x1A, 0x53, 0x59, 0x24, 0xF5, 0x44, 
	0x7A, 0xA5, 0x02, 0x54, 0x01, 0x36, 0x75, 0x36, 0x77, 0xD0, 0x57, 0x35, 0x98, 0x11, 0x4D, 0xD0, 
	0x83, 0x70, 0x86, 0x63, 0x41, 0x67, 0xE3, 0x26, 0xA6, 0xD1, 0x3D, 0x2B, 0xF4, 0x38, 0x1E, 0xB4, 
	0x42, 0x72, 0xA7, 0x20, 0xEF, 0x95, 0x28, 0xC8, 0x25, 0x10, 0x56, 0x27, 0x10, 0x24, 0xE5, 0x10, 
	0xFE, 0xC6, 0x59, 0xE1, 0x92, 0x43, 0xD8, 0x42, 0x19, 0xBE, 0x73, 0x52, 0xE0, 0x21, 0x26, 0x6A, 
	0xFF, 0xC2, 0x21, 0xEE, 0x85, 0x2A, 0x92, 0x10, 0x3E, 0x10, 0x13, 0x2B, 0x5F, 0xD6, 0x40, 0x74, 
	0xD4, 0x21, 0xED, 0x21, 0x7D, 0x09, 0xF4, 0x23, 0xC7, 0xD6, 0x78, 0x8B, 0x04, 0x45, 0xE0, 0xD2, 
	0x39, 0x37, 0xF1, 0x20, 0x51, 0xE5, 0x32, 0xD1, 0x54, 0x6C, 0x56, 0x42, 0x21, 0xA6, 0x31, 0x14, 
	0x68, 0x88, 0x19, 0x03, 0x41, 0x3F, 0x85, 0x13, 0x00, 0xFF, 0xC4, 0x49, 0x20, 0x13, 0x32, 0x2A, 
	0x20, 0x87, 0x10, 0xC2, 0x53, 0x27, 0xD3, 0x5D, 0xF4, 0x70, 0x50, 0xAE, 0x53, 0x4D, 0x1D, 0xB7, 
	0x59, 0x0B, 0xE3, 0x23, 0x08, 0x54, 0x1B, 0x51, 0x44, 0x11, 0x04, 0x93, 0x39, 0x23, 0x54, 0x19, 
	0x4A, 0xE2, 0x20, 0x14, 0x24, 0x09, 0xC3, 0xF8, 0x37, 0x4A, 0xE6, 0x23, 0x6A, 0x62, 0x37, 0xD3, 
	0x28, 0x09, 0x9F, 0x30, 0x36, 0xA3, 0x03, 0x03, 0x25, 0xB3, 0x40, 0x3D, 0x14, 0x5E, 0xA2, 0xF1, 
	0x7F, 0x95, 0x35, 0x27, 0x0F, 0x31, 0x59, 0x50, 0xE3, 0x33, 0xDB, 0xE2, 0x54, 0xA4, 0x07, 0x24, 
	0xDC, 0xD4, 0x5D, 0xAD, 0xD1, 0x72, 0xAB, 0xA4, 0x2C, 0x57, 0xC8, 0x8E, 0x3C, 0x23, 0x81, 0x47, 
	0x31, 0x4D, 0xDC, 0x83, 0x19, 0xBF, 0xA5, 0x2E, 0x19, 0xA2, 0x33, 0xC9, 0xE3, 0x50, 0x47, 0x13, 
	0x34, 0xAE, 0x61, 0x00, 0xA5, 0x13, 0x3D, 0x95, 0x45, 0x74, 0x1F, 0xB2, 0x4A, 0x99, 0xF2, 0x87, 
	0xBF, 0xD1, 0x30, 0xC4, 0xF0, 0x78, 0xBA, 0xF6, 0x63, 0x8E, 0x66, 0x37, 0xFC, 0xC1, 0x3C, 0x21, 
	0xC3, 0x70, 0x72, 0x73, 0x3A, 0x89, 0x43, 0x21, 0xEE, 0xA5, 0x0C, 0xF9, 0x00, 0x5A, 0x8C, 0xB6, 
	0x3A, 0xAF, 0xD7, 0x38, 0x9B, 0x01, 0x28, 0xF4, 0xE0, 0x25, 0x9A, 0xE1, 0x45, 0x11, 0xF2, 0x82, 
	0xC0, 0x66, 0x23, 0x3A, 0xB3, 0x72, 0x42, 0x33, 0x7E, 0xB9, 0x87, 0x36, 0x00, 0x44, 0x82, 0xDC, 
	0xFF, 0xE5, 0x8B, 0x33, 0xA1, 0x64, 0x89, 0x93, 0x29, 0x17, 0x18, 0x1E, 0x4A, 0x15, 0x90, 0x0C, 
	0x37, 0x75, 0x92, 0xA5, 0x8A, 0xA6, 0x21, 0x5D, 0x29, 0x72, 0x1D, 0x19, 0x04, 0x35, 0x39, 0xB3, 
	0x42, 0x36, 0xA3, 0x24, 0x21, 0xA2, 0x4F, 0x99, 0x13, 0x2B, 0x62, 0x42, 0x5B, 0x26, 0x03, 0x7E, 
	0xE7, 0x38, 0x40, 0xD1, 0xA3, 0x1B, 0x7F, 0x35, 0x1A, 0x06, 0x12, 0x06, 0xAA, 0x91, 0x92, 0x0E, 
	0xA3, 0x3B, 0xD0, 0xE1, 0x1F, 0x93, 0x91, 0x5B, 0x30, 0x90, 0x96, 0xA0, 0x61, 0x81, 0xA4, 0xF1, 
	0x49, 0x11, 0x72, 0x5F, 0xC2, 0xD1, 0x48, 0x8A, 0x63, 0x22, 0xB9, 0x15, 0x06, 0x37, 0x80, 0x44, 
	0xB0, 0xD4, 0x53, 0xC4, 0x60, 0x20, 0xBA, 0x41, 0x63, 0x07, 0x61, 0x4E, 0x8D, 0x13, 0x03, 0xD0, 
	0xA1, 0x02, 0x21, 0x36, 0x1A, 0xE9, 0xE1, 0x28, 0xEB, 0x82, 0x4E, 0xB9, 0x42, 0x61, 0xE9, 0x61, 
	0x3D, 0x05, 0x31, 0x59, 0x99, 0xA1, 0x19, 0xDC, 0xD1, 0x26, 0x42, 0x95, 0x44, 0xE3, 0x11, 0x79, 
	0x58, 0x53, 0x0F, 0x9A, 0x97, 0x3B, 0x00, 0x90, 0x8B, 0x14, 0x32, 0x89, 0x36, 0xC6, 0x18, 0x8A, 
	0x38, 0x0C, 0x82, 0xB9, 0x22, 0x1E, 0x39, 0x19, 0xFC, 0xC5, 0x1D, 0x81, 0x55, 0x17, 0x0B, 0xA6, 
	0x50, 0x9E, 0xC5, 0x28, 0x49, 0x72, 0x67, 0xA0, 0x04, 0x50, 0x91, 0xB3, 0x31, 0x62, 0xB9, 0x2E, 
	0x7E, 0x13, 0x8C, 0x0F, 0x72, 0x54, 0x59, 0x83, 0x2A, 0x8B, 0x84, 0x29, 0x6D, 0x57, 0x17, 0x59, 
	0xC8, 0x75, 0x69, 0x71, 0x13, 0x0F, 0xE7, 0x7B, 0xFD, 0x04, 0x2C, 0x8A, 0x55, 0x99, 0xD8, 0xB2, 
	0x7E, 0x9A, 0x43, 0x2F, 0x59, 0xF8, 0x30, 0x40, 0xF5, 0x4A, 0xD8, 0x15, 0x86, 0x82, 0xC3, 0x23, 
	0x4E, 0x15, 0x29, 0x70, 0xD8, 0x25, 0x9A, 0xB7, 0x8D, 0xB4, 0x95, 0x40, 0xAF, 0xD6, 0x8A, 0xEC, 
	0xFF, 0x27, 0x2A, 0xEC, 0x31, 0x1F, 0x11, 0x41, 0x57, 0x94, 0x68, 0x4B, 0x09, 0xB3, 0x0F, 0xE4, 
	0xC5, 0x33, 0x0E, 0xD8, 0x64, 0xDB, 0x63, 0x95, 0xCC, 0x11, 0x5B, 0x3B, 0xD4, 0x34, 0xA6, 0x61, 
	0x2E, 0xD6, 0x03, 0x2D, 0x80, 0x89, 0x7B, 0x02, 0x33, 0x40, 0xA9, 0xC3, 0x35, 0x35, 0x72, 0x13, 
	0xEC, 0xC5, 0x34, 0x7A, 0xF3, 0x22, 0xEC, 0xC9, 0x3E, 0x91, 0x33, 0x2E, 0xDD, 0xF2, 0x21, 0xC7, 
	0x09, 0x88, 0xF4, 0xC0, 0x63, 0xF7, 0x08, 0x36, 0xCD, 0xE1, 0x7B, 0xEB, 0xC2, 0x4A, 0x2F, 0xA2, 
	0x1D, 0x62, 0x27, 0x29, 0xAD, 0x31, 0x5A, 0x5C, 0x03, 0x28, 0xB1, 0xB3, 0x27, 0xCB, 0x20, 0x70, 
	0x7E, 0xD7, 0x29, 0xEB, 0x68, 0x10, 0x87, 0xE2, 0x9E, 0xA6, 0x33, 0x24, 0x66, 0xE2, 0x2A, 0x08, 
	0xE7, 0x77, 0x0E, 0x83, 0x32, 0x1A, 0x8A, 0x2D, 0xCC, 0xC1, 0x53, 0xB6, 0x74, 0x76, 0x89, 0x43, 
	0x30, 0x92, 0x34, 0x26, 0x14, 0xD1, 0x30, 0xCE, 0x24, 0x56, 0x0C, 0xA2, 0x52, 0xEF, 0xF7, 0x39, 
	0xD7, 0x97, 0x6F, 0x03, 0xC3, 0x32, 0xAE, 0x42, 0x30, 0x4B, 0x62, 0x1B, 0x43, 0xEA, 0x2D, 0x28, 
	0x4A, 0x1B, 0x58, 0xC1, 0x53, 0xFA, 0x40, 0x9F, 0x16, 0x53, 0x79, 0xEC, 0xA7, 0x50, 0x6E, 0xB7, 
	0x3D, 0xEA, 0x72, 0x79, 0x28, 0x59, 0x30, 0xB4, 0xF5, 0x89, 0x09, 0xC1, 0x74, 0xEA, 0xB9, 0x32, 
	0xD9, 0xA1, 0x2F, 0xD9, 0x11, 0x0D, 0xB1, 0x84, 0x32, 0x4A, 0x0A, 0xA3, 0xD6, 0x83, 0x59, 0xDD, 
	0xE2, 0x76, 0x88, 0x26, 0x29, 0x32, 0x65, 0x7C, 0x1C, 0x72, 0x54, 0x93, 0x77, 0x76, 0x12, 0x01, 
	0x7D, 0xA4, 0xE2, 0xA5, 0xC9, 0x05, 0x9F, 0xAD, 0xF2, 0x22, 0x4E, 0xF2, 0x70, 0x1B, 0xBA, 0x53, 
	0x9C, 0xA2, 0x53, 0x02, 0xC3, 0x8E, 0x1D, 0xEA, 0x39, 0x9A, 0xE9, 0x3B, 0x8C, 0x65, 0x1A, 0x31, 
	0xF8, 0xB6, 0x24, 0x20, 0xC8, 0x3D, 0x4E, 0x22, 0x10, 0xB6, 0x38, 0x79, 0xEB, 0xA2, 0x39, 0x32, 
	0x9A, 0x2A, 0x0D, 0x0A, 0x11, 0x75, 0xC8, 0x49, 0x3E, 0xA8, 0x99, 0x68, 0x86, 0x15, 0xDD, 0x12, 
	0x69, 0x16, 0x52, 0x1B, 0xA4, 0x2A, 0x2A, 0xB7, 0x51, 0xAA, 0x07, 0x9A, 0x1D, 0x6E, 0x87, 0x22, 
	0x9D, 0x02, 0x88, 0x92, 0x1A, 0x54, 0xDA, 0x01, 0x6E, 0xBA, 0x37, 0xA8, 0xAA, 0xBA, 0x10, 0x3D, 
	0xA2, 0x99, 0x16, 0xD3, 0xA6, 0x9A, 0x19, 0x9C, 0x04, 0x41, 0x79, 0xF7, 0x69, 0x75, 0x04, 0x07, 
	0x8A, 0x2C, 0x63, 0xA1, 0xB4, 0xEA, 0xAA, 0x88, 0x97, 0x1D, 0x21, 0xDA, 0xA6, 0x76, 0x3A, 0xAB, 
	0x62, 0x75, 0x9C, 0x3D, 0x62, 0x1B, 0x0D, 0x43, 0x7C, 0x49, 0x3A, 0xA5, 0x16, 0x13, 0xA9, 0x76, 
	0x2A, 0x11, 0xE1, 0xD5, 0x76, 0xB6, 0xC1, 0x9E, 0x30, 0xC6, 0x77, 0x31, 0xD6, 0x23, 0xF3, 0xB1, 
	0xAB, 0xD0, 0xDA, 0x83, 0x80, 0x98, 0xA9, 0x72, 0x05, 0x1C, 0xB2, 0x36, 0x10, 0xB7, 0xF7, 0xA9, 
	0x99, 0x1A, 0x11, 0x38, 0xE5, 0xA3, 0x54, 0x4A, 0x17, 0xCD, 0x51, 0x79, 0x75, 0x66, 0x9E, 0xFD, 
	0xB9, 0xA7, 0x84, 0xAA, 0x10, 0x9D, 0x62, 0xAF, 0x5F, 0xAA, 0x7F, 0x0A, 0x81, 0x53, 0xDB, 0x4A, 
	0xA5, 0xE1, 0x67, 0x82, 0x94, 0x77, 0xAA, 0x41, 0x65, 0x4E, 0x18, 0x91, 0xAF, 0xEA, 0xFA, 0xA5, 
	0x6C, 0x61, 0xAD, 0xBD, 0x1A, 0x14, 0xA1, 0x27, 0xB0, 0xE0, 0xC5, 0x10, 0xED, 0xAA, 0x10, 0x41, 
	0xD1, 0xAA, 0x0E, 0xD1, 0xAA, 0xE6, 0xBA, 0x10, 0x57, 0x95, 0xAD, 0x92, 0xFA, 0x67, 0x4E, 0x9A, 
	0x10, 0xC7, 0xE9, 0x70, 0xD7, 0x9A, 0xAF, 0x81, 0xA8, 0x13, 0x28, 0x9B, 0xB2, 0x2A, 0xBB, 0xB2, 
	0x2C, 0xDB, 0xB2, 0x2E, 0xFB, 0xB2, 0x06, 0x11, 0x10, 0x00, 0x3B
};

static struct fakefat32_file_t httpd_root_dir[] =
{
	{
		.memfile.file.name = "index.htm",
		.memfile.file.size = sizeof(index_html),
		.memfile.file.attr = VSFILE_ATTR_ARCHIVE | VSFILE_ATTR_READONLY,
		.memfile.f.buff = (uint8_t *)index_html,
	},
	{
		.memfile.file.name = "login.htm",
		.memfile.file.size = sizeof(login_html),
		.memfile.file.attr = VSFILE_ATTR_ARCHIVE | VSFILE_ATTR_READONLY,
		.memfile.f.buff = (uint8_t *)login_html,
	},
	{
		.memfile.file.name = "loginsuccess.htm",
		.memfile.file.size = sizeof(loginsuccess_html),
		.memfile.file.attr = VSFILE_ATTR_ARCHIVE | VSFILE_ATTR_READONLY,
		.memfile.f.buff = (uint8_t *)loginsuccess_html,
	},
	{
		.memfile.file.name = "loginfail.htm",
		.memfile.file.size = sizeof(loginfail_html),
		.memfile.file.attr = VSFILE_ATTR_ARCHIVE | VSFILE_ATTR_READONLY,
		.memfile.f.buff = (uint8_t *)loginfail_html,
	},
	{
		.memfile.file.name = "logo2.jpg",
		.memfile.file.size = sizeof(webimg_logo2),
		.memfile.file.attr = VSFILE_ATTR_ARCHIVE | VSFILE_ATTR_READONLY,
		.memfile.f.buff = (uint8_t *)webimg_logo2,
	},
	{
		.memfile.file.name = NULL,
	},
};

#endif		// __HTTPD_FS_H_INCLUDED__
