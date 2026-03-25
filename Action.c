Action()
{
    web_set_sockets_option("SSL_VERSION", "AUTO");
    web_set_max_html_param_len("8192");
    web_cleanup_cookies();

    // ══════════════════════════════════════════════════════════════
    //  CREDENCIALES (parametrizar con CSV para 10 usuarios)
    // ══════════════════════════════════════════════════════════════
    lr_save_string("rogeliosavar@gmail.com", "username");

    // ══════════════════════════════════════════════════════════════
    //  PASO 1: Home — Porsche México
    // ══════════════════════════════════════════════════════════════
    lr_start_transaction("01_Home");
    web_url("mexico",
        "URL=https://www.porsche.com/mexico/",
        "Resource=0", "RecContentType=text/html",
        "Referer=", "Mode=HTML", LAST);
    lr_end_transaction("01_Home", LR_AUTO);

    lr_think_time(8);

    // ══════════════════════════════════════════════════════════════
    //  PASO 2: Silent Auth (sin sesión, falla silenciosamente)
    // ══════════════════════════════════════════════════════════════
    lr_start_transaction("02_Silent_Auth");
    web_url("authorize",
        "URL=https://identity.porsche.com/authorize?client_id=wMY17MOXY4qBaE2fprbV9Ut4fMN3hjGl&scope=openid+pid%3Auser_profile.name%3Aread+pid%3Auser_profile.porscheid%3Aread&audience=https%3A%2F%2Fapi.porsche.com&prompt=none&response_type=code&response_mode=web_message&state=WmhLbmtFNHFFWWt0SE5ZWmVqRmVrTnViUFUtQ1hUVG1teVJSR2R2VjZqcg%3D%3D&nonce=bDFEY2x%2BV04yLmJ4Mk1wckFTZ2tWZVNZWi1BdVU3ZE52QTlZNkxPRXl5VA%3D%3D&redirect_uri=https%3A%2F%2Fwww.porsche.com&code_challenge=BNvf665pnfKjeI24oiurejFZCWxsnmHAWZkyVrjp02Q&code_challenge_method=S256&auth0Client=eyJuYW1lIjoiYXV0aDAtc3BhLWpzIiwidmVyc2lvbiI6IjIuMy4wIn0%3D",
        "Resource=0", "RecContentType=text/html",
        "Referer=https://www.porsche.com/", "Mode=HTML", LAST);
    lr_end_transaction("02_Silent_Auth", LR_AUTO);

    // i18n del formulario Auth0
    web_url("es_es.json",
        "URL=https://assets.identity.porsche.com/acul-screens/assets/i18n/es_es.json",
        "Resource=0", "RecContentType=application/json", "Referer=", "Mode=HTML", LAST);

    web_url("en_gb.json",
        "URL=https://assets.identity.porsche.com/acul-screens/assets/i18n/en_gb.json",
        "Resource=0", "RecContentType=application/json", "Referer=", "Mode=HTML", LAST);

    // ══════════════════════════════════════════════════════════════
    //  PASO 3: Authorize Login — Correlacionar loginState
    // ══════════════════════════════════════════════════════════════
    web_reg_save_param_ex(
        "ParamName=loginState",
        "LB=login/identifier?state=", "RB=&",
        "Ordinal=1", "NotFound=ERROR",
        SEARCH_FILTERS, "Scope=ALL", LAST);

    lr_start_transaction("03_Authorize_Login");
    web_url("authorize_2",
        "URL=https://identity.porsche.com/authorize?client_id=wMY17MOXY4qBaE2fprbV9Ut4fMN3hjGl&scope=openid+pid%3Auser_profile.name%3Aread+pid%3Auser_profile.porscheid%3Aread&audience=https%3A%2F%2Fapi.porsche.com&ui_locales=es&ext-country=mx&ext-language=es_MX&redirect_uri=https%3A%2F%2Fwww.porsche.com%2Fauth%2Fcallback.html&response_type=code&response_mode=query&state=T2s0RmNpTzEuMkZ6NlA2WS02ZXcuQnVGOWw0NjNoWEppZzNoaDZ1QmNlQw%3D%3D&nonce=NjBjYW9aT240Ym5kTVFZdkNpOWNmNmIxaHp%2BWH5DUVBtY1N6aGF4aVhRUQ%3D%3D&code_challenge=uQQ7h_tIk0iX4gm2QKdDMn-qNbgrlYarXxiXecMO9SE&code_challenge_method=S256&auth0Client=eyJuYW1lIjoiYXV0aDAtc3BhLWpzIiwidmVyc2lvbiI6IjIuMTcuMSJ9",
        "Resource=0", "RecContentType=text/html",
        "Referer=https://www.porsche.com/", "Mode=HTML", LAST);
    lr_end_transaction("03_Authorize_Login", LR_AUTO);

    lr_log_message(">> loginState: %s", lr_eval_string("{loginState}"));
    lr_think_time(5);

    // ══════════════════════════════════════════════════════════════
    //  PASO 4: Submit Username — Correlacionar passwordState
    // ══════════════════════════════════════════════════════════════
    web_reg_save_param_ex(
        "ParamName=passwordState",
        "LB=login/password?state=", "RB=&",
        "Ordinal=1", "NotFound=ERROR",
        SEARCH_FILTERS, "Scope=ALL", LAST);

    web_add_header("Origin", "https://identity.porsche.com");

    lr_start_transaction("04_Submit_Username");
    web_submit_data("identifier",
        "Action=https://identity.porsche.com/u/login/identifier?state={loginState}&ui_locales=es",
        "Method=POST", "RecContentType=text/html",
        "Referer=https://identity.porsche.com/u/login/identifier?state={loginState}&ui_locales=es",
        "Mode=HTML", "EncodeAtSign=YES",
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
    lr_think_time(4);

    // ══════════════════════════════════════════════════════════════
    //  PASO 5: Submit Password — Correlacionar callbackCode/State
    // ══════════════════════════════════════════════════════════════
    web_reg_save_param_ex(
        "ParamName=callbackCode",
        "LB=callback.html?code=", "RB=&",
        "Ordinal=1", "NotFound=ERROR",
        SEARCH_FILTERS, "Scope=ALL", LAST);

    web_reg_save_param_ex(
        "ParamName=callbackState",
        "LB=&state=", "RB=\r\n",
        "Ordinal=1", "NotFound=ERROR",
        SEARCH_FILTERS, "Scope=HEADERS", LAST);

    lr_save_string(lr_unmask("69c34931a5f6ac26607b1a776bff70a0e567518e8a"), "PasswordParameter");

    web_add_header("Origin", "https://identity.porsche.com");

    lr_start_transaction("05_Submit_Password");
    web_submit_data("password",
        "Action=https://identity.porsche.com/u/login/password?state={passwordState}&ui_locales=es",
        "Method=POST", "RecContentType=text/html",
        "Referer=https://identity.porsche.com/u/login/password?state={passwordState}&ui_locales=es",
        "Mode=HTML", "EncodeAtSign=YES",
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

    // ══════════════════════════════════════════════════════════════
    //  PASO 6: Intercambiar code por token (PKCE)
    // ══════════════════════════════════════════════════════════════
    lr_start_transaction("06_Get_Token");
    web_submit_data("token_2",
        "Action=https://identity.porsche.com/oauth/token",
        "Method=POST", "RecContentType=application/json",
        "Referer=https://www.porsche.com/", "Mode=HTML",
        ITEMDATA,
        "Name=client_id",     "Value=wMY17MOXY4qBaE2fprbV9Ut4fMN3hjGl",            ENDITEM,
        "Name=code_verifier", "Value=c1VPhdmh2tWRqzDxlymAHO2Rwh.RzPNflTMr0rpQ2nZ",  ENDITEM,
        "Name=grant_type",    "Value=authorization_code",                             ENDITEM,
        "Name=code",          "Value={callbackCode}",                                 ENDITEM,
        "Name=redirect_uri",  "Value=https://www.porsche.com/auth/callback.html",     ENDITEM,
        LAST);
    lr_end_transaction("06_Get_Token", LR_AUTO);

    // ══════════════════════════════════════════════════════════════
    //  PASO 7: Home Logueado
    // ══════════════════════════════════════════════════════════════
    lr_start_transaction("07_Home_LoggedIn");
    web_url("mexico_2",
        "URL=https://www.porsche.com/mexico/",
        "Resource=0", "RecContentType=text/html",
        "Referer=https://www.porsche.com/auth/callback.html?code={callbackCode}&state={callbackState}",
        "Mode=HTML", LAST);
    lr_end_transaction("07_Home_LoggedIn", LR_AUTO);

    lr_think_time(5);

    // ══════════════════════════════════════════════════════════════
    //  PASO 8: Silent Auth post-login — Correlacionar silentCode1
    // ══════════════════════════════════════════════════════════════
    web_reg_save_param_ex(
        "ParamName=silentCode1",
        "LB=\"code\":\"", "RB=\"",
        "Ordinal=1", "NotFound=ERROR",
        SEARCH_FILTERS, "Scope=BODY", LAST);

    web_url("authorize_3",
        "URL=https://identity.porsche.com/authorize?client_id=wMY17MOXY4qBaE2fprbV9Ut4fMN3hjGl&scope=openid+pid%3Auser_profile.name%3Aread+pid%3Auser_profile.porscheid%3Aread&audience=https%3A%2F%2Fapi.porsche.com&prompt=none&response_type=code&response_mode=web_message&state=VU1BVVF2Lkp%2Bc1p5SXNGNUsxUG1hRTBsMExNQ3R%2BcTRZR0wuX0xlUkIwfg%3D%3D&nonce=Q0poTVhaTlRwVFBpMlUydlE1bzduZFdNSThmZUNDWUtVdnZPLlIxTGFVVQ%3D%3D&redirect_uri=https%3A%2F%2Fwww.porsche.com&code_challenge=4BnqDs-g2YSS3HMUVTASgsrkgs4Uz8mOR5c61rJ-TKs&code_challenge_method=S256&auth0Client=eyJuYW1lIjoiYXV0aDAtc3BhLWpzIiwidmVyc2lvbiI6IjIuMy4wIn0%3D",
        "Resource=0", "RecContentType=text/html",
        "Referer=https://www.porsche.com/", "Mode=HTML", LAST);

    lr_log_message(">> silentCode1: %s", lr_eval_string("{silentCode1}"));

    lr_start_transaction("08_Token_PostLogin");
    web_submit_data("token_3",
        "Action=https://identity.porsche.com/oauth/token",
        "Method=POST", "RecContentType=application/json",
        "Referer=https://www.porsche.com/", "Mode=HTML",
        ITEMDATA,
        "Name=client_id",     "Value=wMY17MOXY4qBaE2fprbV9Ut4fMN3hjGl",            ENDITEM,
        "Name=code_verifier", "Value=dzbkh3OWxsslLBZBF1n6PQWCieJhbusiYJ81Vmletic",  ENDITEM,
        "Name=code",          "Value={silentCode1}",                                  ENDITEM,
        "Name=grant_type",    "Value=authorization_code",                             ENDITEM,
        "Name=redirect_uri",  "Value=https://www.porsche.com",                        ENDITEM,
        LAST);
    lr_end_transaction("08_Token_PostLogin", LR_AUTO);

    // ══════════════════════════════════════════════════════════════
    //  PASO 9: User Information
    // ══════════════════════════════════════════════════════════════
    lr_start_transaction("09_User_Info");
    web_url("user-information",
        "URL=https://resources-nav.porsche.services/user-information",
        "Resource=0", "RecContentType=application/json",
        "Referer=https://www.porsche.com/", "Mode=HTML", LAST);
    lr_end_transaction("09_User_Info", LR_AUTO);

    // Usuario navega el menú hacia Concesionario
    lr_think_time(10);

    // ══════════════════════════════════════════════════════════════
    //  PASO 10: Ver Concesionario — Porsche Centre Guadalajara
    //  El usuario navega desde el menú directamente al dealer.
    //  dealer.porsche.com NO tiene protección Vercel, funciona OK.
    // ══════════════════════════════════════════════════════════════
    lr_start_transaction("10_View_Dealer");
    web_url("dealer_guadalajara",
        "URL=https://dealer.porsche.com/mx/guadalajara/es-MX",
        "Resource=0", "RecContentType=text/html",
        "Referer=https://www.porsche.com/",
        "Mode=HTML", LAST);
    lr_end_transaction("10_View_Dealer", LR_AUTO);

    lr_log_message(">> Dealer Locator completado exitosamente!");

    return 0;
}