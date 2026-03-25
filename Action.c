Action()
{
    web_set_sockets_option("SSL_VERSION", "AUTO");
    web_set_max_html_param_len("8192");
    web_cleanup_cookies();

    // --------------------------------------------------------------
    //  CREDENCIALES
    // --------------------------------------------------------------
    lr_save_string("rogeliosavar@gmail.com", "username");

    // --------------------------------------------------------------
    //  PASO 1: Home — Porsche México
    // --------------------------------------------------------------
    lr_start_transaction("01_Home");
    web_url("mexico",
        "URL=https://www.porsche.com/mexico/",
        "Resource=0",
        "RecContentType=text/html",
        "Referer=",
        "Mode=HTML",
        LAST);
    lr_end_transaction("01_Home", LR_AUTO);

    lr_think_time(2);

    // --------------------------------------------------------------
    //  PASO 2: Silent Auth (sin sesión, falla silenciosamente)
    // --------------------------------------------------------------
    lr_start_transaction("02_Silent_Auth");
    web_url("authorize",
        "URL=https://identity.porsche.com/authorize?client_id=wMY17MOXY4qBaE2fprbV9Ut4fMN3hjGl&scope=openid+pid%3Auser_profile.name%3Aread+pid%3Auser_profile.porscheid%3Aread&audience=https%3A%2F%2Fapi.porsche.com&prompt=none&response_type=code&response_mode=web_message&state=Sk0zTFh3QXVpcExEanZLbmJpMlhnUW9nMHdXZ1lxN2RES2xBMVRIOHJRYg%3D%3D&nonce=SDBmYXNmck94LmFRLlJyUFZxT3pXWWRmUE8yeDNsbm5SbUlJa1cuenJjTw%3D%3D&redirect_uri=https%3A%2F%2Fwww.porsche.com&code_challenge="
        "9VJbqCqzAfpecjII71PRP6Z9vPHzxQuoxy8fGEQtb1Y&code_challenge_method=S256&auth0Client=eyJuYW1lIjoiYXV0aDAtc3BhLWpzIiwidmVyc2lvbiI6IjIuMy4wIn0%3D",
        "Resource=0",
        "RecContentType=text/html",
        "Referer=https://www.porsche.com/",
        "Mode=HTML",
        LAST);
    lr_end_transaction("02_Silent_Auth", LR_AUTO);

    // --------------------------------------------------------------
    //  PASO 3: i18n del formulario Auth0
    // --------------------------------------------------------------
    web_url("es_es.json",
        "URL=https://assets.identity.porsche.com/acul-screens/assets/i18n/es_es.json",
        "Resource=0",
        "RecContentType=application/json",
        "Referer=",
        "Mode=HTML",
        LAST);

    web_url("en_gb.json",
        "URL=https://assets.identity.porsche.com/acul-screens/assets/i18n/en_gb.json",
        "Resource=0",
        "RecContentType=application/json",
        "Referer=",
        "Mode=HTML",
        LAST);

    // --------------------------------------------------------------
    //  PASO 4: Authorize Login — Correlacionar loginState
    // --------------------------------------------------------------
    web_reg_save_param_ex(
        "ParamName=loginState",
        "LB=login/identifier?state=",
        "RB=&",
        "Ordinal=1",
        "NotFound=ERROR",
        SEARCH_FILTERS,
        "Scope=ALL",
        LAST);

    lr_start_transaction("03_Authorize_Login");
    web_url("authorize_2",
        "URL=https://identity.porsche.com/authorize?client_id=wMY17MOXY4qBaE2fprbV9Ut4fMN3hjGl&scope=openid+pid%3Auser_profile.name%3Aread+pid%3Auser_profile.porscheid%3Aread&audience=https%3A%2F%2Fapi.porsche.com&ui_locales=es&ext-country=mx&ext-language=es_MX&redirect_uri=https%3A%2F%2Fwww.porsche.com%2Fauth%2Fcallback.html&response_type=code&response_mode=query&state=Z3RqOXFMZDBGU2lHNVJlYUNWdmVRTnJOM1NYaklXZlFxbmVfX1VSaHdnTg%3D%3D&nonce=S3VIOWs3cmJVZkJmR1dEYmtjLW81ckNUanlTYy5rbnU2Z0lFeF81NFhRcQ%3D%3D&"
        "code_challenge=JpBLT7u8vURYlQ7lrd9e-n1wHh2d7jlhWnHz6FvNanE&code_challenge_method=S256&auth0Client=eyJuYW1lIjoiYXV0aDAtc3BhLWpzIiwidmVyc2lvbiI6IjIuMy4wIn0%3D",
        "Resource=0",
        "RecContentType=text/html",
        "Referer=https://www.porsche.com/",
        "Mode=HTML",
        LAST);
    lr_end_transaction("03_Authorize_Login", LR_AUTO);

    lr_log_message(">> loginState: %s", lr_eval_string("{loginState}"));

    // --------------------------------------------------------------
    //  PASO 5: Submit Username — Correlacionar passwordState
    // --------------------------------------------------------------
    web_reg_save_param_ex(
        "ParamName=passwordState",
        "LB=login/password?state=",
        "RB=&",
        "Ordinal=1",
        "NotFound=ERROR",
        SEARCH_FILTERS,
        "Scope=ALL",
        LAST);

    web_add_header("Origin", "https://identity.porsche.com");

    lr_start_transaction("04_Submit_Username");
    web_submit_data("identifier",
        "Action=https://identity.porsche.com/u/login/identifier?state={loginState}&ui_locales=es",
        "Method=POST",
        "RecContentType=text/html",
        "Referer=https://identity.porsche.com/u/login/identifier?state={loginState}&ui_locales=es",
        "Mode=HTML",
        "EncodeAtSign=YES",
        ITEMDATA,
        "Name=username",                    "Value={username}",   ENDITEM,
        "Name=captcha",                     "Value=",             ENDITEM,
        "Name=js-available",                "Value=true",         ENDITEM,
        "Name=is-brave",                    "Value=false",        ENDITEM,
        "Name=webauthn-available",          "Value=true",         ENDITEM,
        "Name=webauthn-platform-available", "Value=true",         ENDITEM,
        "Name=allow-passkeys",              "Value=true",         ENDITEM,
        "Name=state",                       "Value={loginState}", ENDITEM,
        "Name=acul-sdk",                    "Value=@auth0/auth0-acul-js@1.2.0", ENDITEM,
        LAST);
    lr_end_transaction("04_Submit_Username", LR_AUTO);

    lr_log_message(">> passwordState: %s", lr_eval_string("{passwordState}"));

    // --------------------------------------------------------------
    //  PASO 6: Submit Password — Correlacionar callbackCode/State
    // --------------------------------------------------------------
    web_reg_save_param_ex(
        "ParamName=callbackCode",
        "LB=callback.html?code=",
        "RB=&",
        "Ordinal=1",
        "NotFound=ERROR",
        SEARCH_FILTERS,
        "Scope=ALL",
        LAST);

    web_reg_save_param_ex(
        "ParamName=callbackState",
        "LB=&state=",
        "RB=\r\n",
        "Ordinal=1",
        "NotFound=ERROR",
        SEARCH_FILTERS,
        "Scope=HEADERS",
        LAST);

    lr_save_string(lr_unmask("69b5a937e869bce816b043b81e438b5c040f109a46"), "PasswordParameter");

    web_add_header("Origin", "https://identity.porsche.com");

    lr_start_transaction("05_Submit_Password");
    web_submit_data("password",
        "Action=https://identity.porsche.com/u/login/password?state={passwordState}&ui_locales=es",
        "Method=POST",
        "RecContentType=text/html",
        "Referer=https://identity.porsche.com/u/login/password?state={passwordState}&ui_locales=es",
        "Mode=HTML",
        "EncodeAtSign=YES",
        ITEMDATA,
        "Name=username",                    "Value={username}",          ENDITEM,
        "Name=password",                    "Value={PasswordParameter}", ENDITEM,
        "Name=captcha",                     "Value=",                    ENDITEM,
        "Name=js-available",                "Value=true",                ENDITEM,
        "Name=is-brave",                    "Value=false",               ENDITEM,
        "Name=webauthn-available",          "Value=true",                ENDITEM,
        "Name=webauthn-platform-available", "Value=true",                ENDITEM,
        "Name=allow-passkeys",              "Value=true",                ENDITEM,
        "Name=state",                       "Value={passwordState}",     ENDITEM,
        "Name=acul-sdk",                    "Value=@auth0/auth0-acul-js@1.2.0", ENDITEM,
        LAST);
    lr_end_transaction("05_Submit_Password", LR_AUTO);

    lr_log_message(">> callbackCode: %s", lr_eval_string("{callbackCode}"));
    lr_log_message(">> callbackState: %s", lr_eval_string("{callbackState}"));

    // --------------------------------------------------------------
    //  PASO 7: Intercambiar code por token (PKCE)
    // --------------------------------------------------------------
    lr_start_transaction("06_Get_Token");
    web_submit_data("token_2",
        "Action=https://identity.porsche.com/oauth/token",
        "Method=POST",
        "RecContentType=application/json",
        "Referer=https://www.porsche.com/",
        "Mode=HTML",
        ITEMDATA,
        "Name=client_id",     "Value=wMY17MOXY4qBaE2fprbV9Ut4fMN3hjGl",         ENDITEM,
        "Name=code_verifier", "Value=fbqyvI8cTJpys3UdPqobwDC9ZTPZlpCaW5TaRK4_6Jb", ENDITEM,
        "Name=grant_type",    "Value=authorization_code",                          ENDITEM,
        "Name=code",          "Value={callbackCode}",                              ENDITEM,
        "Name=redirect_uri",  "Value=https://www.porsche.com/auth/callback.html",  ENDITEM,
        LAST);
    lr_end_transaction("06_Get_Token", LR_AUTO);

    // --------------------------------------------------------------
    //  PASO 8: Home Logueado
    // --------------------------------------------------------------
    lr_start_transaction("07_Home_LoggedIn");
    web_url("mexico_2",
        "URL=https://www.porsche.com/mexico/",
        "Resource=0",
        "RecContentType=text/html",
        "Referer=https://www.porsche.com/auth/callback.html?code={callbackCode}&state={callbackState}",
        "Mode=HTML",
        LAST);
    lr_end_transaction("07_Home_LoggedIn", LR_AUTO);

    // --------------------------------------------------------------
    //  PASO 9: Silent Auth post-login — Correlacionar silentCode
    //  (response_mode=web_message ? code viene en el body)
    // --------------------------------------------------------------
    web_reg_save_param_ex(
        "ParamName=silentCode1",
        "LB=\"code\":\"",
        "RB=\"",
        "Ordinal=1",
        "NotFound=ERROR",
        SEARCH_FILTERS,
        "Scope=BODY",
        LAST);

    web_url("authorize_3",
        "URL=https://identity.porsche.com/authorize?client_id=wMY17MOXY4qBaE2fprbV9Ut4fMN3hjGl&scope=openid+pid%3Auser_profile.name%3Aread+pid%3Auser_profile.porscheid%3Aread&audience=https%3A%2F%2Fapi.porsche.com&prompt=none&response_type=code&response_mode=web_message&state=MmFSVktmRmc3c2gzQWxySjZWbGFPMklnSk5Mc1FwREJmbC5scnRfTnVKQw%3D%3D&nonce=T2RnYThxT2VGM3Q0NjZnWW5JVkJSVm9GNXl1ZDJFTFRPVlZuei5zY2dPZA%3D%3D&redirect_uri=https%3A%2F%2Fwww.porsche.com&code_challenge="
        "OaYsZgGb7Ocg30t2m_UfXuBO8i1hPMSI66aQesLdDAg&code_challenge_method=S256&auth0Client=eyJuYW1lIjoiYXV0aDAtc3BhLWpzIiwidmVyc2lvbiI6IjIuMy4wIn0%3D",
        "Resource=0",
        "RecContentType=text/html",
        "Referer=https://www.porsche.com/",
        "Mode=HTML",
        LAST);

    lr_log_message(">> silentCode1: %s", lr_eval_string("{silentCode1}"));

    lr_start_transaction("08_Token_PostLogin");
    web_submit_data("token_3",
        "Action=https://identity.porsche.com/oauth/token",
        "Method=POST",
        "RecContentType=application/json",
        "Referer=https://www.porsche.com/",
        "Mode=HTML",
        ITEMDATA,
        "Name=client_id",     "Value=wMY17MOXY4qBaE2fprbV9Ut4fMN3hjGl",         ENDITEM,
        "Name=code_verifier", "Value=9ImwM-qE47fDDbEJJZp1Vlkol~Z6eLMjfc8LcDdIA1C", ENDITEM,
        "Name=code",          "Value={silentCode1}",                               ENDITEM,
        "Name=grant_type",    "Value=authorization_code",                          ENDITEM,
        "Name=redirect_uri",  "Value=https://www.porsche.com",                     ENDITEM,
        LAST);
    lr_end_transaction("08_Token_PostLogin", LR_AUTO);

    // --------------------------------------------------------------
    //  PASO 10: User Information
    // --------------------------------------------------------------
    lr_start_transaction("09_User_Info");
    web_url("user-information",
        "URL=https://resources-nav.porsche.services/user-information",
        "Resource=0",
        "RecContentType=application/json",
        "Referer=https://www.porsche.com/",
        "Mode=HTML",
        LAST);
    lr_end_transaction("09_User_Info", LR_AUTO);

    lr_think_time(3);

    // --------------------------------------------------------------
    //  PASO 11: Navegar a 911 Carrera
    // --------------------------------------------------------------
    lr_start_transaction("10_Navigate_911");
    web_url("911-carrera",
        "URL=https://www.porsche.com/mexico/models/911/carrera-models/911-carrera/",
        "Resource=0",
        "RecContentType=text/html",
        "Referer=https://www.porsche.com/mexico/",
        "Mode=HTML",
        LAST);
    lr_end_transaction("10_Navigate_911", LR_AUTO);

    // Silent auth para 911 page
    web_reg_save_param_ex(
        "ParamName=silentCode2",
        "LB=\"code\":\"",
        "RB=\"",
        "Ordinal=1",
        "NotFound=ERROR",
        SEARCH_FILTERS,
        "Scope=BODY",
        LAST);

    web_url("authorize_4",
        "URL=https://identity.porsche.com/authorize?client_id=wMY17MOXY4qBaE2fprbV9Ut4fMN3hjGl&scope=openid+pid%3Auser_profile.name%3Aread+pid%3Auser_profile.porscheid%3Aread&audience=https%3A%2F%2Fapi.porsche.com&prompt=none&response_type=code&response_mode=web_message&state=anVySGRoQm9GRUgualNjcEQtNlNONHFkeVlub3dtMkRVOTRiSUFwY3hIYQ%3D%3D&nonce=VXh2ME8zWXI4RGlXYkRKSWdJOTRVMThtcH5NcXIuMnRlYX5CUE95b0gtNw%3D%3D&redirect_uri=https%3A%2F%2Fwww.porsche.com&code_challenge="
        "z35PPCumNdAEVa_FrILoEAwFyfqs4cxWUb0_W3Dyq4E&code_challenge_method=S256&auth0Client=eyJuYW1lIjoiYXV0aDAtc3BhLWpzIiwidmVyc2lvbiI6IjIuMy4wIn0%3D",
        "Resource=0",
        "RecContentType=text/html",
        "Referer=https://www.porsche.com/",
        "Mode=HTML",
        LAST);

    web_submit_data("token_4",
        "Action=https://identity.porsche.com/oauth/token",
        "Method=POST",
        "RecContentType=application/json",
        "Referer=https://www.porsche.com/",
        "Mode=HTML",
        ITEMDATA,
        "Name=client_id",     "Value=wMY17MOXY4qBaE2fprbV9Ut4fMN3hjGl",         ENDITEM,
        "Name=code_verifier", "Value=zwbdvTEYiJqg9TGD96fd.LkIZbI6ItRa9.SCzTOOQqD", ENDITEM,
        "Name=code",          "Value={silentCode2}",                               ENDITEM,
        "Name=grant_type",    "Value=authorization_code",                          ENDITEM,
        "Name=redirect_uri",  "Value=https://www.porsche.com",                     ENDITEM,
        LAST);

    web_url("user-information_3",
        "URL=https://resources-nav.porsche.services/user-information",
        "Resource=0",
        "RecContentType=application/json",
        "Referer=https://www.porsche.com/",
        "Mode=HTML",
        LAST);

    lr_think_time(2);

    // --------------------------------------------------------------
    //  PASO 12: Abrir Configurador 911 Carrera
    // --------------------------------------------------------------
    lr_start_transaction("11_Open_Configurator");
    web_url("9921B2",
        "URL=https://configurator.porsche.com/es-MX/mode/model/9921B2",
        "Resource=0",
        "RecContentType=text/html",
        "Referer=https://www.porsche.com/",
        "Mode=HTML",
        LAST);
    lr_end_transaction("11_Open_Configurator", LR_AUTO);

    // Silent auth configurador (client wMY17)
    web_reg_save_param_ex(
        "ParamName=silentCode3",
        "LB=\"code\":\"",
        "RB=\"",
        "Ordinal=1",
        "NotFound=ERROR",
        SEARCH_FILTERS,
        "Scope=BODY",
        LAST);

    web_url("authorize_5",
        "URL=https://identity.porsche.com/authorize?client_id=wMY17MOXY4qBaE2fprbV9Ut4fMN3hjGl&scope=openid+pid%3Auser_profile.name%3Aread+pid%3Auser_profile.porscheid%3Aread&audience=https%3A%2F%2Fapi.porsche.com&prompt=none&response_type=code&response_mode=web_message&state=ZV85Sld3clczaHVFY0RiZlNUV0xFWEQuTGhsZEw0SzFEQ1ZCQVRhRXp2LQ%3D%3D&nonce=eTd%2BNk1ZWlYyUExHWVhWLlZTWHdXT3ZBTENXWDhuUGtLYllnd3QuM1l6SA%3D%3D&redirect_uri=https%3A%2F%2Fconfigurator.porsche.com&code_challenge="
        "OHYr_QkoZOl7KEoWn0rYYz_gPWng9cz06nqgoapio3k&code_challenge_method=S256&auth0Client=eyJuYW1lIjoiYXV0aDAtcmVhY3QiLCJ2ZXJzaW9uIjoiMi44LjAifQ%3D%3D",
        "Resource=0",
        "RecContentType=text/html",
        "Referer=https://configurator.porsche.com/",
        "Mode=HTML",
        LAST);

    web_submit_data("token_6",
        "Action=https://identity.porsche.com/oauth/token",
        "Method=POST",
        "RecContentType=application/json",
        "Referer=https://configurator.porsche.com/",
        "Mode=HTML",
        ITEMDATA,
        "Name=client_id",     "Value=wMY17MOXY4qBaE2fprbV9Ut4fMN3hjGl",         ENDITEM,
        "Name=code_verifier", "Value=nebrcx1WkicOhn.27l3MQ8H1Ql~KLuLp1lt7p4MVoT0", ENDITEM,
        "Name=code",          "Value={silentCode3}",                               ENDITEM,
        "Name=grant_type",    "Value=authorization_code",                          ENDITEM,
        "Name=redirect_uri",  "Value=https://configurator.porsche.com",            ENDITEM,
        LAST);

    // Silent auth configurador (client DjHxa — para login del configurador)
    web_reg_save_param_ex(
        "ParamName=configCode1",
        "LB=\"code\":\"",
        "RB=\"",
        "Ordinal=1",
        "NotFound=ERROR",
        SEARCH_FILTERS,
        "Scope=BODY",
        LAST);

    web_url("authorize_6",
        "URL=https://identity.porsche.com/authorize?client_id=DjHxaZFPfyHAWvfgV8VmQoAJowNrSXlM&scope=openid+profile+email&redirect_uri=https%3A%2F%2Fconfigurator.porsche.com%2Flogin-callback&audience=https%3A%2F%2Fapi.porsche.com&ui_locales=es-MX&ext-language=es_MX&ext-country=MX&ext-target-uri=https%3A%2F%2Fporsche-code.com%2F&prompt=none&response_type=code&response_mode=web_message&state=bjdSWDlwUWw3aGJ2UHRUaE5rWmk2R2l4SS40ZE5yeV9oZF91OHd1ckhJQg%3D%3D&nonce="
        "TEUwMk9vZEYxekN5MGxkYkF5TFNrVzhMZGFNMVJiLjByN1kzZUMzTVBEag%3D%3D&code_challenge=vUNyaMdXTeQwRYFaqIBTxKGbx2FZJ7ODQb08_TGD-nc&code_challenge_method=S256&auth0Client=eyJuYW1lIjoiYXV0aDAtcmVhY3QiLCJ2ZXJzaW9uIjoiMi4xNC4wIn0%3D",
        "Resource=0",
        "RecContentType=text/html",
        "Referer=https://configurator.porsche.com/",
        "Mode=HTML",
        LAST);

    web_url("user-information_5",
        "URL=https://resources-nav.porsche.services/user-information?locale=es-MX",
        "Resource=0",
        "RecContentType=application/json",
        "Referer=https://configurator.porsche.com/",
        "Mode=HTML",
        LAST);

    lr_think_time(2);

    // --------------------------------------------------------------
    //  PASO 13: Configurar — Seleccionar Color Exterior (Rojo Guardia)
    // --------------------------------------------------------------
    lr_start_transaction("12_Config_Color");
    web_submit_data("9921B2.data",
        "Action=https://configurator.porsche.com/es-MX/mode/model/9921B2.data",
        "Method=POST",
        "RecContentType=text/x-script",
        "Referer=https://configurator.porsche.com/es-MX/mode/model/9921B2",
        "Mode=HTML",
        ITEMDATA,
        "Name=needle",           "Value=",        ENDITEM,
        "Name=mode",             "Value=customer", ENDITEM,
        "Name=locale",           "Value=es-MX",   ENDITEM,
        "Name=orderType",        "Value=9921B2",  ENDITEM,
        "Name=modelYear",        "Value=",        ENDITEM,
        "Name=dealershipId",     "Value=",        ENDITEM,
        "Name=presentationMode", "Value=model",   ENDITEM,
        "Name=options-IIR",      "Value=Q5A",     ENDITEM,
        "Name=optionAdded",      "Value=G1",      ENDITEM,
        "Name=options", "Value=1PE", ENDITEM,
        "Name=options", "Value=7HK", ENDITEM,
        "Name=options", "Value=6FG", ENDITEM,
        "Name=options", "Value=Q5A", ENDITEM,
        "Name=options", "Value=IV5", ENDITEM,
        "Name=options", "Value=QH1", ENDITEM,
        "Name=options", "Value=VL0", ENDITEM,
        "Name=options", "Value=G1F", ENDITEM,
        "Name=options", "Value=PH",  ENDITEM,
        "Name=options", "Value=0Q",  ENDITEM,
        LAST);
    lr_end_transaction("12_Config_Color", LR_AUTO);

    lr_think_time(2);

    // --------------------------------------------------------------
    //  PASO 14: Configurar — Seleccionar Rines RS Spyder
    // --------------------------------------------------------------
    lr_start_transaction("13_Config_Wheels");
    web_submit_data("9921B2.data_2",
        "Action=https://configurator.porsche.com/es-MX/mode/model/9921B2.data",
        "Method=POST",
        "RecContentType=text/x-script",
        "Referer=https://configurator.porsche.com/es-MX/mode/model/9921B2",
        "Mode=HTML",
        ITEMDATA,
        "Name=needle",           "Value=",        ENDITEM,
        "Name=mode",             "Value=customer", ENDITEM,
        "Name=locale",           "Value=es-MX",   ENDITEM,
        "Name=orderType",        "Value=9921B2",  ENDITEM,
        "Name=modelYear",        "Value=",        ENDITEM,
        "Name=dealershipId",     "Value=",        ENDITEM,
        "Name=presentationMode", "Value=model",   ENDITEM,
        "Name=options-IIR",      "Value=Q5A",     ENDITEM,
        "Name=optionAdded",      "Value=59C",     ENDITEM,
        "Name=options", "Value=1PE", ENDITEM,
        "Name=options", "Value=7HK", ENDITEM,
        "Name=options", "Value=6FG", ENDITEM,
        "Name=options", "Value=Q5A", ENDITEM,
        "Name=options", "Value=G1F", ENDITEM,
        "Name=options", "Value=PH",  ENDITEM,
        "Name=options", "Value=G1",  ENDITEM,
        LAST);
    lr_end_transaction("13_Config_Wheels", LR_AUTO);

    lr_think_time(2);

    // --------------------------------------------------------------
    //  PASO 15: Configurar — Seleccionar Interior Rojo Burdeos
    // --------------------------------------------------------------
    lr_start_transaction("14_Config_Interior");
    web_submit_data("9921B2.data_3",
        "Action=https://configurator.porsche.com/es-MX/mode/model/9921B2.data",
        "Method=POST",
        "RecContentType=text/x-script",
        "Referer=https://configurator.porsche.com/es-MX/mode/model/9921B2",
        "Mode=HTML",
        ITEMDATA,
        "Name=needle",           "Value=",        ENDITEM,
        "Name=mode",             "Value=customer", ENDITEM,
        "Name=locale",           "Value=es-MX",   ENDITEM,
        "Name=orderType",        "Value=9921B2",  ENDITEM,
        "Name=modelYear",        "Value=",        ENDITEM,
        "Name=dealershipId",     "Value=",        ENDITEM,
        "Name=presentationMode", "Value=model",   ENDITEM,
        "Name=options-IIR",      "Value=Q5A",     ENDITEM,
        "Name=optionAdded",      "Value=LX",      ENDITEM,
        "Name=options", "Value=1PE", ENDITEM,
        "Name=options", "Value=6FG", ENDITEM,
        "Name=options", "Value=Q5A", ENDITEM,
        "Name=options", "Value=G1F", ENDITEM,
        "Name=options", "Value=59C", ENDITEM,
        "Name=options", "Value=PH",  ENDITEM,
        "Name=options", "Value=G1",  ENDITEM,
        LAST);
    lr_end_transaction("14_Config_Interior", LR_AUTO);

    lr_think_time(2);

    // --------------------------------------------------------------
    //  PASO 16: Guardar Configuración — Auth para save
    //  (Se necesita un nuevo token del cliente DjHxa con id_token)
    // --------------------------------------------------------------
    web_reg_save_param_ex(
        "ParamName=configCode2",
        "LB=\"code\":\"",
        "RB=\"",
        "Ordinal=1",
        "NotFound=ERROR",
        SEARCH_FILTERS,
        "Scope=BODY",
        LAST);

    web_url("authorize_7",
        "URL=https://identity.porsche.com/authorize?client_id=DjHxaZFPfyHAWvfgV8VmQoAJowNrSXlM&scope=openid+profile+email&redirect_uri=https%3A%2F%2Fconfigurator.porsche.com%2Flogin-callback&audience=https%3A%2F%2Fapi.porsche.com&ui_locales=es-MX&ext-language=es_MX&ext-country=MX&ext-target-uri=https%3A%2F%2Fporsche-code.com%2F&prompt=none&response_type=code&response_mode=web_message&state=SVNqbk9EVH5XTGlJWlBidjNIWE1xRmthMEMxV3lWN2VhZ3A3MVBuN3FUZQ%3D%3D&nonce="
        "V0Y5N3IyZjVIMl9BWjNoZFQxc1RIZ0Y3QTB2MGs3Ym9YdVkxQkttcFI1dg%3D%3D&code_challenge=Eu1nujA4kJOO2ltKzEqf-CKjDOljGzujOw7LGDwNFpY&code_challenge_method=S256&auth0Client=eyJuYW1lIjoiYXV0aDAtcmVhY3QiLCJ2ZXJzaW9uIjoiMi4xNC4wIn0%3D",
        "Resource=0",
        "RecContentType=text/html",
        "Referer=https://configurator.porsche.com/",
        "Mode=HTML",
        LAST);

    lr_log_message(">> configCode2: %s", lr_eval_string("{configCode2}"));

    // Intercambiar por token con id_token (capturar porscheIdToken)
    web_reg_save_param_ex(
        "ParamName=porscheIdToken",
        "LB=\"id_token\":\"",
        "RB=\"",
        "Ordinal=1",
        "NotFound=ERROR",
        SEARCH_FILTERS,
        "Scope=BODY",
        LAST);

    web_submit_data("token_7",
        "Action=https://identity.porsche.com/oauth/token",
        "Method=POST",
        "RecContentType=application/json",
        "Referer=https://configurator.porsche.com/",
        "Mode=HTML",
        ITEMDATA,
        "Name=client_id",       "Value=DjHxaZFPfyHAWvfgV8VmQoAJowNrSXlM",            ENDITEM,
        "Name=redirect_uri",    "Value=https://configurator.porsche.com/login-callback", ENDITEM,
        "Name=ui_locales",      "Value=es-MX",                                         ENDITEM,
        "Name=ext-language",    "Value=es_MX",                                         ENDITEM,
        "Name=ext-country",     "Value=MX",                                            ENDITEM,
        "Name=ext-target-uri",  "Value=https://porsche-code.com/",                     ENDITEM,
        "Name=code_verifier",   "Value=PIME0KV2FZt8aTebZH3yWSz7E_dkSfUsvyxU4_QAend",  ENDITEM,
        "Name=code",            "Value={configCode2}",                                  ENDITEM,
        "Name=grant_type",      "Value=authorization_code",                             ENDITEM,
        LAST);

    lr_log_message(">> porscheIdToken (first 50 chars): %.50s...", lr_eval_string("{porscheIdToken}"));

    // --------------------------------------------------------------
    //  PASO 17: Guardar Configuración (Save Porsche Code)
    // --------------------------------------------------------------
    lr_start_transaction("15_Save_Configuration");
    web_submit_data("save-porsche-code.data",
        "Action=https://configurator.porsche.com/es-MX/mode/model/9921B2/save-porsche-code.data?options=1PE.6FG.Q5A.IV5.QH1.VL0.1X2.6EE.3S0.9P9.8T1.4L6.GT5.3HA.2C5.K8C.7V0.6XV.0I3.9WT.4UF.1NA.QJ6.6F0.8LH.3QT.2V1.1G8.6BA.FT3.7M9.4F2.1P0.6RC.9JA.9VL.7K3.0TD.6JD.3UG.95A.1T2.5XC.6NJ.G1F.3FA.6I3.3J7.KQ1.3B3.2ZC.VR0.IW3.FM0.VC2.E0A.7UG.1BH.VF0.D8R.0NB.2W0.UP1.9AD.4A3.9ZE.0P0.7X5.8IT.EM2.7AK.4GP.5MA.KA6.FZ0.1LS.8VG.8N7.QQ2.UI2.1N1.GV1.LX.59C.G1",
        "Method=POST",
        "RecContentType=text/x-script",
        "Referer=https://configurator.porsche.com/es-MX/mode/model/9921B2/save-porsche-code",
        "Mode=HTML",
        ITEMDATA,
        "Name=porscheIdToken", "Value={porscheIdToken}", ENDITEM,
        LAST);
    lr_end_transaction("15_Save_Configuration", LR_AUTO);

    lr_log_message(">> Configuracion guardada exitosamente!");

    return 0;
}