package com.xProject.net
{
	public class PolicyFile
	{
		public static const FILE_CONTENT:String="<?xml version=\"1.0\"?>" +
			"<cross-domain-policy>" +
			"<site-control permitted-cross-domain-policies=\"all\"/>" +
			"<allow-access-from domain=\"*\" to-ports=\"9000\"/>" +
			"</cross-domain-policy>";
	}
}