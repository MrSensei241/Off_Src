#include "stdafx.h"
#include "Form1.h"


namespace TOSLoginFaceBook {

	void AddQueryItem(String^* str, String^ queryType, String^ queryValue)
	{
		*str += ("&" + queryType + "=" + queryValue);
	}

	String^ GetInnerTagText(String^ str, String^ tag)
	{
		String^ startTag = "<" + tag + ">";
		String^ endTag= "</" + tag + ">";
		int startIndex = str->IndexOf(startTag) + startTag->Length;
		int endIndex = str->IndexOf(endTag);
		String^ ret = str->Substring(startIndex, endIndex - startIndex);
		return ret;
	}
	
	void	Form1::Init(String^ binPath, String^ dataPath)
	{
		m_binPath = binPath;
		m_dataPath = dataPath;
		m_authState = "None";
		m_appSecret = "5550c896258b5d92aa35700ae5036bec";		
		m_webUrl = "https://facebook.com/dialog/oauth?client_id=";
		m_appID = "598100156955583";
		String^ redirectionUrl = "http://www.facebook.com/connect/login_success.html";

		String^ url = m_webUrl + m_appID + "&redirect_uri=" + redirectionUrl;
		AddQueryItem(&url, "scope", "user_groups,read_friendlists,user_friends,user_photos");
		AddQueryItem(&url, "display", "popup");

		//LOGS(url);
		this->m_web->Navigate(url);
	}
	
	System::Void Form1::webBrowser1_DocumentCompleted(System::Object^  sender, System::Windows::Forms::WebBrowserDocumentCompletedEventArgs^  e)
	{
		// LOGS(webBrowser1->Document->Url);
	}

	System::Void Form1::webBrowser1_Navigated(System::Object^  sender, System::Windows::Forms::WebBrowserNavigatedEventArgs^  e)
	{
		String^ url = m_web->Document->Url->ToString();

		if (m_authState == "None")
		{
			String^ successToken = "connect/login_success.html?code=";
			if (url->Contains(successToken))
			{
				int index = url->IndexOf(successToken);
				index += successToken->Length;
				String^ token = url->Substring(index, url->Length - index);
				///LOGS(token);
				m_graphAccessCode = token;

				String^ graphAddr = "https://graph.facebook.com/oauth/access_token?";
				AddQueryItem(&graphAddr, "client_id", m_appID);
				AddQueryItem(&graphAddr, "redirect_uri", "http://www.facebook.com/connect/login_success.html");
				AddQueryItem(&graphAddr, "client_secret", m_appSecret);
				AddQueryItem(&graphAddr, "code", m_graphAccessCode);
				m_web->Navigate(graphAddr);
				m_authState = "Graph";
			}
			else 
			{
				// LOGS("Login");
				// LOGS(url);
			}
		}
		else if (m_authState == "Graph")
		{
			String^ docText = m_web->DocumentText;
			String^ accessToken = GetInnerTagText(docText, "PRE");
			String^ key = "access_token=";
			if (accessToken->Contains(key))
			{
				accessToken = accessToken->Substring(key->Length, accessToken->Length - key->Length);
				m_authState = "Complete";

				String^ tokenPath = m_binPath + "tos_token";
				LOGS(tokenPath);
				
				System::IO::StreamWriter^ sw = gcnew System::IO::StreamWriter(tokenPath);
				sw->WriteLine(accessToken);
				sw->Close();
				Close();

				/*
				String^ newUrl = "https://graph.facebook.com/me/taggable_friends?";
				// String^ newUrl = "https://graph.facebook.com/me/friends?";
				//String^ newUrl = "https://graph.facebook.com/721917261205278?";
				// String^ newUrl = "https://graph.facebook.com/454985964604835?";
				AddQueryItem(&newUrl, "access_token", accessToken);
				m_web->Navigate(newUrl);*/
			}
			else 
			{
				LOGS("Error");
			}
		}
		else if (m_authState == "Complete")
		{
			
		}
		
	}

};