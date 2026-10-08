"""Testes de integracao no Linux: python3 testes/testar.py. Nao altera dados reais."""
from pathlib import Path
import hashlib, subprocess, tempfile
RAIZ = Path(__file__).resolve().parents[1]
with tempfile.TemporaryDirectory() as pasta:
    d = Path(pasta)
    exe = d/'driver'
    fontes = [str(p) for p in (RAIZ/'src').glob('*.c') if p.name != 'main.c']
    subprocess.run(['gcc', '-std=c11', '-Wall', '-Wextra', '-Wpedantic', '-Werror', '-I', str(RAIZ/'include'), *fontes, str(RAIZ/'testes/driver.c'), '-lcrypto', '-o', str(exe)], check=True)
    def run(acao, entrada='', *args, code=0):
        r = subprocess.run([str(exe), acao, *args], input=entrada, text=True, capture_output=True, cwd=d, timeout=20)
        assert r.returncode == code, (acao, r.returncode, r.stdout, r.stderr)
        return r.stdout
    def ler(nome): return (d/nome).read_text()
    def gravar(nome, texto): (d/nome).write_text(texto)
    h = run('hash').strip().split('$')
    assert hashlib.pbkdf2_hmac('sha256', b'Senha123', bytes.fromhex(h[2]), int(h[1])).hex() == h[3]
    run('bloqueio')
    run('cadastro', 'Teste\nteste@example.com\nSenha123\nDiferente\n')
    assert not (d/'usuarios.txt').exists()
    run('cadastro', 'Teste\nteste@example.com\nSenha123\nSenha123\n')
    assert 'pbkdf2$600000$' in ler('usuarios.txt') and '|Senha123|' not in ler('usuarios.txt')
    run('login', 'teste@example.com\nSenha123\n0\n')
    assert 'MENU DO CLIENTE' in run('login', 'teste@example.com\nSenha123\n0\n')
    antes = ler('usuarios.txt')
    run('senha', 'teste@example.com\nTeste\nIncorreta\n')
    assert ler('usuarios.txt') == antes
    run('senha', 'teste@example.com\nTeste\nSenha123\nNova123\nNova123\n')
    assert 'MENU DO CLIENTE' in run('login', 'teste@example.com\nNova123\n0\n')
    run('perfil', 'Nome Novo\n', 'teste@example.com')
    assert ler('usuarios.txt').startswith('Nome Novo|')
    # Cliente legado sem quebra de linha final; profissional completo.
    gravar('usuarios.txt', 'Cliente|cli@example.com|abc123|cliente\nProf|pro@example.com|abc123|profissional|52998224725|01/01/1990|Recife|81999999999|Eletrica')
    run('cadastro', 'Outro\noutro@example.com\nSenha123\nSenha123\n')
    assert len(ler('usuarios.txt').splitlines()) == 3
    run('login', 'cli@example.com\nabc123\n0\n')
    assert ler('usuarios.txt').splitlines()[0].split('|')[2].startswith('pbkdf2$')
    run('oferta', '1\nInstalacao\n150,00\n', 'pro@example.com')
    run('oferta', '2\nReparo\n200\n', 'pro@example.com')
    assert len(ler('profissionais.txt').splitlines()) == 2
    antes = ler('profissionais.txt')
    run('anuncio', '1\n2\nEXCLUIR\n', 'outro@example.com')
    assert ler('profissionais.txt') == antes
    run('anuncio', '1\n1\nInstalacao nova\n175\n', 'pro@example.com')
    assert 'Instalacao nova' in ler('profissionais.txt')
    run('anuncio', '2\n2\nEXCLUIR\n', 'pro@example.com')
    assert len(ler('profissionais.txt').splitlines()) == 1
    run('pedido', '1\nRecife\nRua A\n10\nCentro\n-\nTrocar tomada\n', 'cli@example.com')
    assert '|ABERTO|' in ler('tickets.txt')
    run('atualizar', '0001\n', 'outro@example.com', '3')
    assert '|ABERTO|' in ler('tickets.txt')
    run('atualizar', '0001\n', 'pro@example.com', '1')
    assert '|ACEITO|pro@example.com|' in ler('tickets.txt')
    run('atualizar', '0001\n', 'pro@example.com', '2')
    assert '|CONCLUIDO|' in ler('tickets.txt')
    antes = ler('tickets.txt')
    run('atualizar', '0001\n', 'cli@example.com', '3')
    assert ler('tickets.txt') == antes
    run('pedido', '1\nRecife\nRua A\n10\nCentro\n-\nOutro\n', 'cli@example.com')
    run('atualizar', '0002\n', 'cli@example.com', '3')
    assert '|CANCELADO|' in ler('tickets.txt')
    # Edicao da cidade passa a valer para anuncios existentes.
    run('perfil', 'Prof Novo\nOlinda\n81999999999\nInstalacoes\n', 'pro@example.com', 'profissional')
    run('pedido', '1\nOlinda\nRua A\n10\nCentro\n-\nOutro\n', 'cli@example.com')
    run('atualizar', '0003\n', 'pro@example.com', '1')
    assert '|ACEITO|' in ler('tickets.txt').splitlines()[-1]
    run('atualizar', '0003\n', 'pro@example.com', '4')
    assert '|CANCELADO|' in ler('tickets.txt').splitlines()[-1]
    run('integridade')
    antes = ler('tickets.txt')
    gravar('tickets.txt', antes + 'linha danificada\n')
    assert 'linha 4' in run('integridade', code=1)
    assert ler('tickets.txt') == antes + 'linha danificada\n'
    gravar('tickets.txt', antes + antes.splitlines()[0] + '\n')
    assert 'duplicado' in run('integridade', code=1)
    gravar('tickets.txt', antes)
    # Nao substitui arquivo enquanto houver backup pendente.
    gravar('profissionais.txt.backup', 'preservar')
    antes = ler('profissionais.txt')
    run('anuncio', '1\n2\nEXCLUIR\n', 'pro@example.com')
    assert ler('profissionais.txt') == antes
    assert ler('profissionais.txt.backup') == 'preservar'
    print('OK: hash, cadastro, login, migracao, senha, bloqueio, perfil, anuncios, pedidos, permissoes, integridade e backup.')
